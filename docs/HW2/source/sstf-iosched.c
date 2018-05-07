/* Elevator SSTF */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h>
#include <linux/kernel.h>

/*Varaible Definitions*/
#define DRV_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define DRV_DESC "An SSTF (Shortest Seek Time First) I/O scheduler"

/* References */

	/*
	 * [0]: https://www.tldp.org/LDP/lkmpg/2.4/html/book1.htm - module programming
	 * [1]: 
	 */

struct sstf_data {
	struct list_head queue;
}


module_init(sstf_init); //entry point for the kernel to execute the driver at boot time
module_exit(sstf_exit); //exit point for the kernel to exit the driver

MODULE_AUTHOR(DRV_AUTHORS);
MODULE_LICENSE("GPL"); /* General public license */
MODULE_DESCRIPTION(DRV_DESC); /*Description of the Driver*/
