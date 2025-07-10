#include <stdlib.h>
#include <string.h>

static char *_strndup(const char *s, size_t len) {
    if (!s) return NULL;
    char *copy = malloc(len + 1);
    if (copy) memcpy(copy, s, len + 1);
    return copy;
}

static char *_strdup(const char *s) {
    return _strndup(s,  strlen(s));
}