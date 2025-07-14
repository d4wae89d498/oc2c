#ifndef _FD_COPY_H
#define _FD_COPY_H

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

static int _fd_copy(FILE *src, const char *dst_path) {
    // Rewind to the beginning of the file/stream
    rewind(src);

    FILE *dst = fopen(dst_path, "wb");
    if (!dst) {
        perror("fopen dest");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Copy loop
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, dst);
        if (bytes_written < bytes_read) {
            perror("fwrite");
            fclose(dst);
            return -1;
        }
    }

    if (ferror(src)) {
        perror("fread");
        fclose(dst);
        return -1;
    }

    fclose(dst);
    return 0;
}

#endif // _FD_COPY_H
