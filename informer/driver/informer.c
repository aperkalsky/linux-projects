#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/msr.h>	// to read CPU regs
#include <linux/types.h>

#include "informer.h"

void test_temp_read(void)
{
	u64 therm;
	int delta, temperature;

	// read MSR
	rdmsrl(0x19C, therm);
	pr_info("THERM MSR = 0x%llx\n", therm);
	
	// extract temperature delta
	delta = (therm >> 16) & 0x7F;	
	pr_info("Delta = %d\n", delta);
	
	// calculate real CPU temperature
	temperature = TJ_MAX - delta;
	pr_info("CPU temperature = %d\n", temperature);
}

static int __init informer_init(void)
{
    printk(KERN_INFO "informer kernel module initialized\n");
	test_temp_read();
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