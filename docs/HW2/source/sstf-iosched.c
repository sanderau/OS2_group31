/* Elevator SSTF */
/* Authors: Austin Sanders */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h>
#include <linux/kernel.h>

/*Varaible Definitions*/
#define MOD_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define MOD_DESC "An SSTF (Shortest Seek Time First) I/O scheduler"

/* References */

	/*
	 * [0]: https://www.tldp.org/LDP/lkmpg/2.4/html/book1.htm - module programming
	 * [1]: 
	 */

struct sstf_data {
	struct list_head queue;
}

/*			INIT and EXIT functions		*/

/*
 * This function is what actually initializes the module. This is the function
 * called by the kernel when initialized.
 */
static int __init sstf_init(void) {
	//TODO: actually write the init for this module lol
}

/*
 * Clean up function that is called on the modules exit in the kernel 
 */
static void __exit sstf_exit(void) {
	//TODO: write the cleanup function to be called on the modules exit
}

module_init(sstf_init); //entry point for the kernel to execute the driver at boot time
module_exit(sstf_exit); //exit point for the kernel to exit the driver

MODULE_AUTHOR(MOD_AUTHORS);
MODULE_LICENSE("GPL"); /* General public license */
MODULE_DESCRIPTION(MOD_DESC); /*Description of the Driver*/
