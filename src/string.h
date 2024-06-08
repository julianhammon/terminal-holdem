#include <stdlib.h>

typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} String;

String *new_string();
String *new_string_with_capacity(size_t cap);
String *new_string_from_source(char *ptr, size_t len);
void string_free(String *str);