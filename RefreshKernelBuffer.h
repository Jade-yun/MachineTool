#ifndef REFRESH_KERNEL_BUFFER_H
#define REFRESH_KERNEL_BUFFER_H

#include <fcntl.h>
#include <unistd.h>


#define REFRESH_KERNEL_BUFFER(file_path) \
    do { \
        int fd = open((file_path), O_WRONLY); \
        if (fd != -1) { \
            fsync(fd); \
            close(fd); \
        } \
    } while (0)

#endif // REFRESH_KERNEL_BUFFER_H
