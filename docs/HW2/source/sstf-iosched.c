/* Elevator SSTF */
/* Authors: Austin Sanders */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h> /* list_head data structure */
#include <linux/kernel.h>

/*Varaible Definitions*/
#define MOD_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define MOD_DESC "An SSTF (Shortest Seek Time First) I/O scheduler"

/* References */

	/*
	 * [0]: https://www.tldp.org/LDP/lkmpg/2.4/html/book1.htm - module programming
	 * [1]: https://www.elixir.bootlin.com/linux/v3.6/indent/ - where I got all of my information on required libraries 
	 * * Note: I used many pages from this website, but I just referenced the search engine that I used
	 * [2]: 
	 */

struct sstf_data {
	struct list_head queue;
}

/*		Elevator Data Structure		*/

static struct elevator_type elevator_sstf = {
	.ops = {

	},
	.elevator_name = "SSTF",
	.elevator_owner = THIS_MODULE
};


/*		INIT and EXIT functions		*/

/*
 * This function is what actually initializes the module. This is the function
 * called by the kernel when initialized.
 */
static int __init sstf_init(void) {
	return elv_register(&elevator_sstf);	
}

/*
 * Clean up function that is called on the modules exit in the kernel 
 */
static void __exit sstf_exit(void) {
	return elv_unregister(&elevator_sstf);
}

module_init(sstf_init); //entry point for the kernel to execute the driver at boot time
module_exit(sstf_exit); //exit point for the kernel to exit the driver

MODULE_AUTHOR(MOD_AUTHORS);
MODULE_LICENSE("GPL"); /* General public license */
MODULE_DESCRIPTION(MOD_DESC); /*Description of the Driver*/
