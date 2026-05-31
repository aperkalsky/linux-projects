This demo project shows how to use a kernel mode driver to access the HW registers.

For safety the read access is used.

The driver reads CPU temperature from its MSR. It opens read() interface to be accessible
from the user mode app. On load it creates a devnode. The app uses this devnode to open
a driver.

This project was tested on Lenovo laptop running Ubuntu Linux.