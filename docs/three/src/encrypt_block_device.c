#include <linux/blkdev.h> /* Items used: request_queue, */
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h> /* list_head data structure */
#include <linux/kernel.h> /* */

#define MOD_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define MOD_DESC "An SSTF (Shortest Seek Time First) I/O scheduler"

//This is the linux implementation of BRD
//https://github.com/torvalds/linux/blob/master/drivers/block/brd.c

MODULE_LICENSE("Dual BSD/GPL");

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
