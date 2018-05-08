/* Elevator SSTF */
/* Authors: Austin Sanders */

#include <linux/blkdev.h> /* Items used: request_queue, */
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h> /* Module_init and module_exit macros*/
#include <linux/list.h> /* list_head data structure */
#include <linux/kernel.h> /* */

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

/* List that will contain the sector requests */

struct sstf_data {
	struct list_head queue;
};

/*	Functions	*/

/*
 * this is the initailize function for the elevator
 */
static int sstf_init_queue(struct request_queue *rq, struct elevator_type *et)
{
	struct sstf_data *sd;
	struct elevator_queue *eq;

	eq = elevator_alloc(rq, et);

	if(!eq) {
		/*If we cannot allocate memory, return the correct error message */		
		return ENOMEM; 
	}

	sd = kmalloc_node(sizeof(*sd), GFP_KERNEL, rq->node);
	if(!sd) {
		kobject_put(&eq->kobj);
		return ENOMEM; /* if we can not allocate memory for the queue, sent appropriate error code */
	}

	eq->elevator_data=sd;


	INIT_LIST_HEAD(&sd->queue);

	spin_lock_irq(rq->queue_lock);
	rq->elevator = eq;
	spin_unlock_irq(rq->queue_lock);

	printk(KERN_DEBUG "SSTF: INIT queue\n");
	return 0;


}

/*
 * This function is in charge of freeing the memory used by
 * this function
 */
static void sstf_exit_queue(struct elevator_queue *el_queue)
{
	struct sstf_data *data = el_queue->elevator_data;

	BUG_ON(!list_empty(&data->queue));
	kfree(data);
}

/* 
 * This function will add an item to the list for the elevator to deal with at an appropriate time.
 */
static void sstf_add_queue(struct request_queue *rq, struct request *r)
{
	struct sstf_data *data = rq->elevator->elevator_data;
	struct request *curr_req;
	struct list_head *curr_pos;
	
	if(list_empty(&data->queue)) {
		/*Does not matter where the request is if list is empty*/
		printk(KERN_DEBUG "sstf-iosched: adding item to empty list\n");
		list_add(&r->queuelist, &data->queue);
	} else {
		/*iterating through the list*/
		list_for_each(curr_pos, &data->queue) {
			curr_req = list_entry(curr_pos, struct request, queuelist);
			/*if req sector is higher than the current position, then add the item in from of the list*/
			if(blk_rq_pos(curr_req) < blk_rq_pos(r)) {
				printk(KERN_DEBUG "sstf-iosched: adding item to correct pos\n");
				list_add(&r->queuelist, &curr_req->queuelist);
				break;
			}
		}
	}
}

/*
 * merge the requests
 */
static void sstf_merged_req(struct request_queue *rq, struct request *r, struct request *next) 
{
	list_del_init(&next->queuelist);
}

/*
 *dispatch
 */

static int sstf_dispatch(struct request_queue *rq, int force)
{
	struct sstf_data *data = rq->elevator->elevator_data;

	if(!(list_empty(&data->queue))) {
		
		struct request *req = list_entry(data->queue.next, struct request, queuelist);
		list_del_init(&req->queuelist);

		elv_dispatch_add_tail(rq, req);
		return 1;
	}

	return 0;
}

static struct request *
sstf_former_req(struct request_queue *q, struct request *rq)
{
	struct sstf_data *sd = q->elevator->elevator_data;

	if(rq->queuelist.prev == &sd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_req(struct request_queue *q, struct request *rq)
{
	struct sstf_data *sd = q->elevator->elevator_data;

	if(rq->queuelist.next == &sd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}


/*		Elevator Data Structure		*/

static struct elevator_type elevator_sstf = {
	/*
	 * ops - the operations of the newly defined elevator type
	 * elevator_name - name of the newly written elevator
	 * elevator_owner - module that owns this elevator. 
	 * * Prevents premature termination by kerdnel.
	 */
	.ops = {
		.elevator_init_fn 	= sstf_init_queue, /*initialize the elevator */
		.elevator_exit_fn 	= sstf_exit_queue, /*free the elevator*/
		.elevator_add_req_fn 	= sstf_add_queue, /*add an item to the elevators list*/
		.elevator_merge_req_fn 	= sstf_merged_req, /*merge. those. requests. */
		.elevator_dispatch_fn 	= sstf_dispatch, /* */
		.elevator_former_req_fn = sstf_former_req, /* */
		.elevator_latter_req_fn = sstf_latter_req, /* */
	},
	.elevator_name = "SSTF",
	.elevator_owner = THIS_MODULE,
};


/*		INIT and EXIT functions		*/

/*
 * This function is what actually initializes the module. This is the function
 * called by the kernel when initialized.
 */
static int __init sstf_init(void) {
	/* */
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
