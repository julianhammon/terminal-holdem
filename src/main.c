#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <ctype.h>

struct termios orig_attr;

void die(const char *s) {
	perror(s);
	exit(1);
}

void disable_raw_mode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_attr);
}

void enable_raw_mode() {
	tcgetattr(STDIN_FILENO, &orig_attr);
	atexit(disable_raw_mode);

	struct termios raw = orig_attr;

	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8 );
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enable_raw_mode();

	while (1) {
		unsigned char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
			die("read");
		}

		if (c == '\0') {
			continue;
		}

		if (iscntrl(c)) {
			printf("%02x\r\n", c);
		} else {
			printf("%02x ('%c')\r\n", c, c);
		}

		if (c == 'q') {
			break;
		}
	}

return 0;
}