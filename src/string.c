#include <stdlib.h>
#include <string.h>
#include "string.h"

String *new_string() {
    return new_string_with_capacity(16);
}

String *new_string_with_capacity(size_t cap) {
    String *str = malloc(sizeof(String));

    if (str == NULL) {
        return NULL;
    }

    str->cap = cap;
    str->len = 0;
    str->ptr = malloc(cap);

    if (str->ptr == NULL) {
        free(str);
        return NULL;
    }

    return str;
}

String *new_string_from_source(char *ptr, size_t len) {
    String *str = malloc(sizeof(String));

    if (str == NULL) {
        return NULL;
    }

    str->cap = len;
    str->len = len;
    str->ptr = malloc(len);

    if (str->ptr == NULL) {
        free(str);
        return NULL;
    }

    strncpy(str->ptr, ptr, len);

    return str;
}

void string_free(String *str) {
    free(str->ptr);
    free(str);
}

void string_push(String *str, char chr) {
    if (str->cap == str->len) {
        if (str->cap == 0) {
            str->cap = 8;
        }
        str->cap *= 2;
        str->ptr = realloc(str->ptr, str->cap);
        if (str->ptr == NULL) {
            exit(1);
        }
    }
    
    str->ptr[str->len++];
}