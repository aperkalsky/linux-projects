#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <asm/msr.h>	// to read CPU regs
#include <linux/types.h>

#include "informer.h"

static dev_t informer_dev;
static struct cdev informer_cdev;
static struct class *informer_class;


static const struct file_operations informer_fops = {
    .owner = THIS_MODULE,
    .read  = informer_read,
};

// devnode callback
static char *informer_devnode(const struct device *dev,
                              umode_t *mode)
{
    if (mode)
        *mode = 0644;	// make it readable for all, the default is 0600

    return NULL;
}

int read_cpu_temperature(void)
{
	u64 therm;
	int delta, temperature;

	// read MSR
	rdmsrl(IA32_THERM_STATUS, therm);
	pr_info("THERM MSR = 0x%llx\n", therm);
	
	// extract temperature delta
	delta = (therm >> 16) & IA32_THERM_STATUS_MASK;	
	pr_info("Delta = %d\n", delta);
	
	// calculate real CPU temperature
	temperature = TJ_MAX - delta;
	pr_info("CPU temperature = %d\n", temperature);
	
	return temperature;
}

static ssize_t informer_read(struct file *file,
                             char __user *buf,
                             size_t count,
                             loff_t *ppos)
{
    int temp = read_cpu_temperature();

    if (count < sizeof(temp))
        return -EINVAL;

    if (copy_to_user(buf, &temp, sizeof(temp)))
        return -EFAULT;

    return sizeof(temp);
}

static int __init informer_init(void)
{
    int ret;

    /* Allocate major/minor */
    ret = alloc_chrdev_region(&informer_dev,
                              0,      /* first minor */
                              1,      /* count */
                              "informer");
    if (ret)
        return ret;

    /* Initialize cdev */
    cdev_init(&informer_cdev, &informer_fops);

    ret = cdev_add(&informer_cdev,
                   informer_dev,
                   1);
    if (ret)
        goto err_unregister;

    /* Create class */
    informer_class = class_create("informer");
    if (IS_ERR(informer_class)) {
        ret = PTR_ERR(informer_class);
        goto err_cdev;
    }
	
	// register devnode callback
	informer_class->devnode = informer_devnode;

    /* Create device node */
    if (IS_ERR(device_create(informer_class,
                             NULL,
                             informer_dev,
                             NULL,
                             "informer")))
    {
        ret = -ENOMEM;
        goto err_class;
    }

    pr_info("informer loaded\n");
    return 0;

err_class:
    class_destroy(informer_class);

err_cdev:
    cdev_del(&informer_cdev);

err_unregister:
    unregister_chrdev_region(informer_dev, 1);

    return ret;
}

static void __exit informer_exit(void)
{
    device_destroy(informer_class,
                   informer_dev);

    class_destroy(informer_class);

    cdev_del(&informer_cdev);

    unregister_chrdev_region(informer_dev, 1);

    pr_info("informer unloaded\n");
}

module_init(informer_init);
module_exit(informer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alexander Perkalsky");
MODULE_DESCRIPTION("Kernel module to read CPU temperature");