#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef  void* id;

static void *class_createInstance(size_t **size, size_t extra)
{
    void *output = malloc(**size + extra);
    if (!output)
        return NULL;
    memcpy(output, size, **size);
    return output;
}

static void object_dispose(void*ptr) {
    free(ptr);
}