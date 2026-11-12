#ifndef OS_H
#define OS_H

#include <stddef.h>
#include <sys/types.h>

typedef int os_handle_t;

typedef struct
{
    pid_t pid;
} os_process_t;

#define OS_INVALID_HANDLE (-1)
#define OS_PATH_MAX 4096

int os_pipe_create(os_handle_t *read_end, os_handle_t *write_end);

int os_file_open_write(const char *path, os_handle_t *out);

int os_sibling_path(const char *name, char *buf, size_t size);

int os_process_start(const char *exe, os_handle_t in, os_handle_t out, os_process_t *proc);

int os_write_all(os_handle_t h, const void *buf, size_t n);

void os_close(os_handle_t h);

int os_process_wait(os_process_t *proc, const char *label);

#endif