#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/blkdev.h> /* Items used: request_queue, */
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h> /*kmalloc and kfree*/
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h> /* list_head data structure */
#include <linux/kernel.h> /* */
#include <linux/moduleparam.h> /* So we can have command line arguments (for a key) */

#define MOD_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define MOD_DESC "A module to write encrypted data and read then decrypt data from RAM while treating it like a block device."
#define KERNEL_SECTOR_SIZE 512

/*
References:

* [0] - This is the linux implementation of BRD
	** https://github.com/torvalds/linux/blob/master/drivers/block/brd.c
* [1] - This has a simple block driver that we modeled our system off of.
	** https://lwn.net/Articles/58719/
*/

MODULE_LICENSE("Dual BSD/GPL");


/* type definitions */

typedef struct request_queue request_queue_t;

/* end of definitions */

static struct request_queue *Queue;
static int nsectors=1024;

static int hardsect_size = 512;
module_param(hardsect_size, int, 0);
static int major_num = 0;
module_param(major_num, int, 0);
static char *key = "someKey";
static int key_length = 7;
module_param(key, charp, 0000);
module_param(key_length, int, 0);


/*
 * The internal representation of our device.
 */
static struct sbd_device {
    unsigned long size;
    spinlock_t lock;
    u8 *data;
    struct gendisk *gd;
} Device;

static void sbd_readWrite(struct sbd_device *dev, unsigned long sector,
		unsigned long nsect, char *buffer, int write)
{
    unsigned long offset = sector*hardsect_size;
    unsigned long nbytes = nsect*hardsect_size;
    int i;
    char *m[2];
    unsigned long len;
    u8 *src;
    u8 *dest;

    crypto_cipher_setkey(crypt, key, key_length);
    
    if ((offset + nbytes) > dev->size) {
	printk (KERN_NOTICE "TOO LARGE OF A WRITE/READ TO DRIVER (%ld %ld)\n", offset, nbytes);
	return;
    }
    /*this should be where encryption will go I might need to write another encrypt
		https://kernel.readthedocs.io/en/sphinx-samples/crypto-API.html*/
    if(write)
    {
	m[0] = "UNENCRYPTED";
     	m[1] = "ENCRYPTED";
	dst = dev->data + offset;
	src = buffer;
    	/* encrypt buffer and put into dev->data*/

    	/* Write from the buffer to the block*/
    	/*memcpy(dev->data + offset, buffer, nbytes);*/
	for(i = 0; i < nbytes; i+= crypto_cipher_blocksize(crypt));
	{
		crypto_cipher_encrypt_one(crypt, dst+i, src+i);
	}
    }
    else
	{
		m[0] = "UNENCRYPTED";
	     	m[1] = "ENCRYPTED";
		dst = buffer;
		src = dev->data + offset;
		
		/* encrypt buffer and put into dev->data*/

		/*Read from the block into the buffer*/
		/*memcpy(buffer, dev->data + offset, nbytes);*/

		for(i=0; i<nbytes; i += crypto_cipher_blocksize(crypt)) {
			crypto_cipher_decrypt_one(crypt, dst + i, src + i);
		}
	}

	len = nbytes;
	printk("%s:", m[0]);
	while(len--)
		printk(KERN_NOTICE "%u", (unsigned) *src++);

	len = nbytes;
	printk("\n%s:", m[1]);
	while(len--)
		printk(KERN_NOTICE "%u", (unsigned) *dst++);
	printk(KERN_INFO "\n");

}

static void sbd_request(request_queue_t *q)
{
    	struct request *req;
    /*
    https://lwn.net/Articles/333620/
    */
	while ((req = elv_next_request(q)) != NULL) {
		if( ! blk_fs_request(req)) {
			printk(KERN_NOTICE "Skip non-cmd request\n");
			end_request(req, 0);
			continue;
		}
    		
		sbd_readWrite(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req), req->buffer, rq_data_dir(req));

		end_request(req, 1);
	}
}



/*
 * Ioctl.
 */
/* https://github.com/torvalds/linux/blob/master/drivers/block/brd.c */
int sbd_getgeo (struct block_device *block_device, struct hd_geometry * geo)
{
	long size;

	size = Device.size * ( hardsect_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	
	return 0;
}




/*
 * The device operations structure.
 */
static struct block_device_operations sbd_ops = {
    .owner           = THIS_MODULE,
    .getgeo 	     = sbd_getgeo
};

static int __init sbd_init(void)
{
/*
 * Set up our internal device.
 */
    Device.size = nsectors*hardsect_size;

    Device.data = vmalloc(Device.size);
    if (Device.data == NULL)
	return -ENOMEM;
/*
 * Get a request queue.
 */
    Queue = blk_init_queue(sbd_request, &Device.lock);
    if (Queue == NULL)
	    goto out;
    blk_queue_logical_block_size(Queue, hardsect_size);
/*
 * Get registered.
 */
    major_num = register_blkdev(major_num, "sbd");
    if (major_num <= 0) {
	printk(KERN_WARNING "sbd: unable to get major number\n");
	goto out;
    }
/*
	I need to reformat this alloc_disk function
*/
    Device.gd = alloc_disk(16);
    if (! Device.gd)
	goto out_unregister;
    Device.gd->major = major_num;
    Device.gd->first_minor = 0;
    Device.gd->fops = &sbd_ops;
    Device.gd->private_data = &Device;
    strcpy (Device.gd->disk_name, "sbd0");
    set_capacity(Device.gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
    Device.gd->queue = Queue;
    add_disk(Device.gd);

    return 0;

  out_unregister:
    unregister_blkdev(major_num, "sbd");
  out:
    kfree(Device.data);
    return -ENOMEM;
}

static void __exit sbd_exit(void)
{
    del_gendisk(Device.gd);
    put_disk(Device.gd);
    unregister_blkdev(major_num, "sbd");
    blk_cleanup_queue(Queue);
    vfree(Device.data);
}
	
module_init(sbd_init);
module_exit(sbd_exit);
