#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

typedef struct {
	char *data;
	int size;
	int capacity;
} byte_vec;

byte_vec *byte_vec_new(int capacity) {
	byte_vec *vec = malloc(sizeof(byte_vec));
	if (vec == NULL) {
		return NULL;
	}

	vec->data = malloc(capacity);
	if (vec->data == NULL) {
		free(vec);
		return NULL;
	}
	vec->size = 0;
	vec->capacity = capacity;

	return vec;
}

void byte_vec_free(byte_vec *vec) {
	free(vec->data);
	free(vec);
}

void byte_vec_push(byte_vec *vec, char byte) {
	if (vec->size == vec->capacity) {
		vec->capacity <<= 1;
		vec->data = realloc(vec->data, vec->capacity);
		if (vec->data == NULL) {
			fprintf(stderr, "Failed to realloc byte_vec\n");
			exit(1);
		}
	}
	
	vec->data[vec->size] = byte;
	vec->size++;
}

int len_utf8_sym(char byte) {
	if ((byte & 0x80) == 0) {
		return 1;
	} else if ((byte & 0xE0) == 0xC0) {
		return 2;
	} else if ((byte & 0xF0) == 0xE0) {
		return 3;
	} else if ((byte & 0xF8) == 0xF0) {
		return 4;
	} else {
		return 0;
	}
}

void print_card(char *buf, off_t size) {
	int width = 0;
	int height = 0;

	byte_vec *utf8_sym_len = byte_vec_new(64);
	char *p = buf;
	int curr_width = 0;
	while (p < buf + size) {
		if (*p == '\n') {
			if (curr_width == 0) {
				p++;
				break;
			}

			if (height > 0 && curr_width != width) {
				fprintf(stderr, "Inconsistent card width\n");
				byte_vec_free(utf8_sym_len);
				return;
			}

			height++;
			width = curr_width;
			curr_width = 0;
			byte_vec_push(utf8_sym_len, 1);
			p++;
			continue;
		}

		int sym_len = len_utf8_sym(*p);
		if (sym_len == 0) {
			fprintf(stderr, "Invalid UTF-8\n");
			byte_vec_free(utf8_sym_len);
			return;
		}

		byte_vec_push(utf8_sym_len, sym_len);
		p += sym_len;
		curr_width++;
	}

	byte_vec *colors = byte_vec_new(64);
	curr_width = 0;
	int color_height = 0;
	while (p < buf + size) {
		if (*p == '\n') {
			if (curr_width != width) {
				fprintf(stderr, "Inconsistent card width\n");
				byte_vec_free(utf8_sym_len);
				byte_vec_free(colors);
				return;
			}

			color_height++;
			curr_width = 0;
			byte_vec_push(colors, colors->data[colors->size - 1]);
			p++;
			continue;
		}

		switch (*p) {
		case 'r':
			byte_vec_push(colors, 31);
			break;
		case 'g':
			byte_vec_push(colors, 32);
			break;
		case 'y':
			byte_vec_push(colors, 33);
			break;
		case 'b':
			byte_vec_push(colors, 34);
			break;
		case 'm':
			byte_vec_push(colors, 35);
			break;
		case 'c':
			byte_vec_push(colors, 36);
			break;
		case '.':
			byte_vec_push(colors, 39);
			break;
		default:
			fprintf(stderr, "Invalid color\n");
			byte_vec_free(utf8_sym_len);
			byte_vec_free(colors);
			return;
		}

		curr_width++;
		p++;
	}

	if (height != color_height) {
		fprintf(stderr, "Inconsistent card height\n");
		byte_vec_free(utf8_sym_len);
		byte_vec_free(colors);
		return;
	}

	printf("Dimensions: %dx%d\n", width, height);

	p = buf;
	char curr_color = 39;
	for (int i = 0; i < utf8_sym_len->size; i++) {
		if (curr_color != colors->data[i]) {
			printf("\x1B[%dm", colors->data[i]);
			curr_color = colors->data[i];
		}

		int sym_len = utf8_sym_len->data[i];
		char sym[5] = {0};
		strncpy(sym, p, sym_len);
		printf("%s", sym);
		p += sym_len;
	}

	printf("\x1B[0m\n");
	byte_vec_free(utf8_sym_len);
	byte_vec_free(colors);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file>...\n", argv[0]);
		return 1;
	}

	for (int i = 1; i < argc; i++) {
		int fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Failed to open %s\n", argv[i]);
			continue;
		}

		struct stat file_stat;
		if (fstat(fd, &file_stat) < 0) {
			fprintf(stderr, "Failed to stat %s\n", argv[i]);
			close(fd);
			continue;
		}
		off_t size = file_stat.st_size;

		char *buf = malloc(size);
		if (buf == NULL) {
			fprintf(stderr, "Failed to allocate buffer\n");
			close(fd);
			continue;
		}

		if (read(fd, buf, size) != size) {
			fprintf(stderr, "Failed to read %s\n", argv[i]);
			free(buf);
			close(fd);
			continue;
		}

		printf("File: %s\n", argv[i]);
		print_card(buf, size);

		close(fd);
	}

	return 0;
}