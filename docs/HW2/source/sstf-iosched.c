/* Elevator SSTF */

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kernel.h>

/* References */

struct sstf_data {
	struct list_head queue;
}


module_init(sstf_init);
module_exit(sstf_exit);

MODULE_AUTHOR("Austin Sanders");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO Scheduler");
