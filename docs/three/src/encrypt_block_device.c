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

/*
References:

* [0] - This is the linux implementation of BRD
	** https://github.com/torvalds/linux/blob/master/drivers/block/brd.c
* [1] -
*/

MODULE_LICENSE("Dual BSD/GPL");


char *myKey = NULL;

module_param(myKey, charp, 0000);
MODULE_PARM_DESC(mystring, "An optional key for the user to provide");

static int hello_init(void)
{
	//void *kmalloc(size_t size, int flags);
	return 0;
}

static void hello_exit(void)
{

	//kfree(const void* objp);
	return 0;
}

module_init(hello_init);
module_exit(hello_exit);
