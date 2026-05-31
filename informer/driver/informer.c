#include <linux/module.h>
#include <linux/kernel.h>

static int __init informer_init(void)
{
    printk(KERN_INFO "informer kernel module initialized\n");
    return 0;
}

static void __exit informer_exit(void)
{
    printk(KERN_INFO "Informer kernel module is removed\n");
}

module_init(informer_init);
module_exit(informer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Perkalsky");
MODULE_DESCRIPTION("Kernel module to read CPU temperature");