Kernel mode driver (loadable module) that demonstrates read from CPU registers (reads CPU temperature)

To load:
	sudo insmod informer.ko
	
It will create a device node \dev\informer
	
Then run user mode app that opens this node

To unload
	sudo rmmod informer