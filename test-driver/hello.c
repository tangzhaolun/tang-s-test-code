#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");

static int hello_init(void)
{
	printk(KERN_ALERT "Hello, world!\n");
	return 0;
}

static void hello_exit(void)
{
	printk("<1>Goodbye cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
