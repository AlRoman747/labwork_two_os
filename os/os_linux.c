#define _GNU_SOURCE
#include "os.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "errors.h"

int os_pipe_create(os_handle_t *read_end, os_handle_t *write_end)
{
    int fds[2];
    if (pipe2(fds, O_CLOEXEC) < 0)
    {
        err_errno("pipe2");
        return -1;
    }
    *read_end = fds[0];
    *write_end = fds[1];
    return 0;
}

int os_file_open_write(const char *path, os_handle_t *out)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);

    if (fd < 0)
    {
        err_errno(path);
        return -1;
    }
    *out = fd;
    return 0;
}

int os_sibling_path(const char *name, char *buf, size_t size)
{
    ssize_t n = readlink("/proc/self/exe", buf, size - 1);
    char *slash;
    size_t dir_len;

    if (n < 0)
    {
        err_errno("readlink /proc/self/exe");
        return -1;
    }
    buf[n] = '\0';
    slash = strrchr(buf, '/');
    dir_len = slash ? (size_t)(slash - buf) + 1 : 0;
    if (dir_len + strlen(name) + 1 > size)
    {
        fprintf(stderr, "os_sibling_path: слишком длинный путь\n");
        return -1;
    }
    strcpy(buf + dir_len, name);
    return 0;
}

int os_process_start(const char *exe, os_handle_t in, os_handle_t out, os_process_t *proc)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        err_errno("fork");
        return -1;
    }
    if (pid == 0)
    {
        if (dup2(in, STDIN_FILENO) < 0 || dup2(out, STDOUT_FILENO) < 0)
            _exit(126);
        execl(exe, exe, (char *)NULL);
        err_errno(exe);
        _exit(127);
    }
    proc->pid = pid;
    return 0;
}

int os_write_all(os_handle_t h, const void *buf, size_t n)
{
    const char *p = (const char *)buf;

    while (n > 0)
    {
        ssize_t w = write(h, p, n);
        if (w < 0)
        {
            if (errno == EINTR)
                continue;
            err_errno("write");
            return -1;
        }
        p += w;
        n -= (size_t)w;
    }
    return 0;
}

void os_close(os_handle_t h)
{
    if (h != OS_INVALID_HANDLE)
        close(h);
}

int os_process_wait(os_process_t *proc, const char *label)
{
    int status = 0;

    while (waitpid(proc->pid, &status, 0) < 0)
    {
        if (errno == EINTR)
            continue;
        err_errno("waitpid");
        return -1;
    }
    if (WIFEXITED(status))
    {
        if (WEXITSTATUS(status) != 0)
        {
            fprintf(stderr, "%s завершился с кодом %d\n", label, WEXITSTATUS(status));
            return -1;
        }
        return 0;
    }
    if (WIFSIGNALED(status))
    {
        fprintf(stderr, "%s убит сигналом %d\n", label, WTERMSIG(status));
        return -1;
    }
    return -1;
}
