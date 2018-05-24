#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#define MOD_AUTHORS "Austin Sanders, Zachary Tusing, Kevin Talik"
#define MOD_DESC "An SSTF (Shortest Seek Time First) I/O scheduler"

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	void *kmalloc(size_t size, int flags);
	return 0;
}

static void hello_exit(void)
{

	kfree(const void* objp);
}

module_init(hello_init);
module_exit(hello_exit);
