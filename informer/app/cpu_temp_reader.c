#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

static struct termios orig_termios;

void restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

// prepare terminal to non-canonical mode
void prepare_terminal(void)
{
    struct termios new_termios;

    /* Save current settings */
    tcgetattr(STDIN_FILENO, &orig_termios);

    /* Restore on exit */
    atexit(restore_terminal);

    new_termios = orig_termios;

    /* Disable canonical mode and echo */
    new_termios.c_lflag &= ~(ICANON | ECHO);

    /* Return each character immediately */
    new_termios.c_cc[VMIN]  = 0;
    new_termios.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

int main(void)
{
    int fd;
	int temp;


    fd = open("/dev/informer", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

	prepare_terminal();

    printf("Reading CPU temperature. Press 'q' to quit\n");

    while (1) {

        fd_set rfds;
        struct timeval tv;

        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select(STDIN_FILENO + 1,
                         &rfds,
                         NULL,
                         NULL,
                         &tv);

        if (ret > 0) {
            char c;

            if (read(STDIN_FILENO, &c, 1) == 1) {

                if (c == 'q' || c == 'Q')
                    break;

                printf("Key: %c\n", c);
            }
        }

		if (read(fd, &temp, sizeof(temp)) == sizeof(temp))
			printf("CPU temperature = %d\n", temp);
    }

	close(fd);
	return 0;
}