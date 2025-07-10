#include <stdlib.h>
#include <string.h>

static char *_strndup(const char *s, size_t len) {
    if (!s) return NULL;
    char *copy = malloc(len + 1);
    if (!copy) return NULL;

    strncpy(copy, s, len);
    copy[len] = '\0';

    return copy;
}

static char *_strdup(const char *s) {
    return _strndup(s, strlen(s));
}
