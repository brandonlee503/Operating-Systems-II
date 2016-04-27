/*
 * elevator sstf look
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
	int direction;
	sector_t head;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq, struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;
	printk("Look Algorithm: sstf_dispatch() - Starting up dispatch!\n");

	if (!list_empty(&nd->queue)) {
		struct request *rq, *next_rq, *prev_rq;

		// Next request and prev request get the request greater/less than current node
		next_rq = list_entry(nd->queue.next, struct request, queuelist);
		prev_rq = list_entry(nd->queue.prev, struct request, queuelist);

		// Set rq, evaluate the nodes in list
		if (next_rq != prev_rq) {
			printk("sstf_dispatch() - Multiple requests!");

			// Check direction
			if (nd->direction == 0) {
				printk("sstf_dispatch() - Moving backward!\n");

				// Check where next request is located in respect to current request
				if (nd->head > prev_rq->__sector) {
					// Request is farther back
					rq = prev_rq;
				} else {
					// Request is farther up
					nd->direction = 1;
					rq = next_rq;
				}
			} else {
				printk("sstf_dispatch() - Moving forward!\n");

				// Check where next request is located in respect to current request
				if (nd->head < next_rq->__sector) {
					// Request is farther up
					rq = next_rq;
				} else {
					// Request is farther back
					nd->direction = 0;
					rq = prev_rq;
				}
			}
		} else {
			// Only one node in list if next is also prev
			printk("sstf_dispatch() - Only one request!\n");
			rq = next_rq;
		}
		printk("sstf_dispatch() - Running!\n");

		// Delete from queue
		list_del_init(&rq->queuelist);

		// Get read head new position
		nd->head = blk_rq_pos(rq) + blk_rq_sectors(rq);

		// Send elevator the request
		elv_dispatch_add_tail(q, rq);

		printk("sstf_dispatch() - Finished running!\n");
		printk("sstf_dispatch() - SSTF reading: %llu\n", (unsigned long long) rq->__sector);
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
    struct sstf_data *nd = q->elevator->elevator_data;
    struct request *next_rq, *prev_rq;

	printk("Look Algorithm: sstf_add_request() - Starting up add!\n");

    if (list_empty(&nd->queue)) {
		printk("Empty list!\n");

		// Empty list, just add to the request
        list_add(&rq->queuelist, &nd->queue);
    } else {
		printk("Looking for a place for the request!\n");

		// Look for where request could be joined into the request list
        next_rq = list_entry(nd->queue.next, struct request, queuelist);
        prev_rq = list_entry(nd->queue.prev, struct request, queuelist);

		// Interate through list and find exact location to add to
        while (blk_rq_pos(rq) > blk_rq_pos(next_rq)) {
            next_rq = list_entry(next_rq->queuelist.next, struct request, queuelist);
            prev_rq = list_entry(prev_rq->queuelist.prev, struct request, queuelist);
        }

		// Add request to the proper location in list
        list_add(&rq->queuelist, &prev_rq->queuelist);
		printk("Found location!\n");
    }

	printk("SSTF adding: %llu\n", (unsigned long long) rq->__sector);
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}

	nd->head = 0;
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);

	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		 = sstf_merged_requests,
		.elevator_dispatch_fn		 = sstf_dispatch,
		.elevator_add_req_fn		 = sstf_add_request,
		.elevator_former_req_fn		 = sstf_former_request,
		.elevator_latter_req_fn		 = sstf_latter_request,
		.elevator_init_fn		     = sstf_init_queue,
		.elevator_exit_fn		     = sstf_exit_queue,
	},
	.elevator_name = "look",
	.elevator_owner = THIS_MODULE,
};

static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Brandon Lee");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SSTF IO scheduler");
