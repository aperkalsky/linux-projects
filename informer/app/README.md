User-mode app to demonstrate read access to the kernel mode driver. It reads and
displays the CPU temperature until interrupted.

To get the same behavior as top (react immediately to a single keypress without Enter),
you need to put the terminal into non-canonical mode using the POSIX terminal API (termios).

In normal (canonical) mode, the terminal buffers input until Enter is pressed. top, less,
vim, etc., disable canonical mode and read characters directly.