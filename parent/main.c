#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "io_utils.h"
#include "os.h"

static char *ask_name(const char *prompt)
{
    size_t len;
    int err;
    char *s;

    printf("%s", prompt);
    fflush(stdout);
    s = read_line(stdin, &len, 0, &err);
    if (s == NULL || len == 0)
    {
        fprintf(stderr, "parent: не удалось прочитать имя файла\n");
        free(s);
        return NULL;
    }
    return s;
}

static void close_h(os_handle_t *h)
{
    os_close(*h);
    *h = OS_INVALID_HANDLE;
}

int main(void)
{
    char *name1 = NULL, *name2 = NULL;
    os_handle_t file1 = OS_INVALID_HANDLE, file2 = OS_INVALID_HANDLE;
    os_handle_t p1r = OS_INVALID_HANDLE, p1w = OS_INVALID_HANDLE;
    os_handle_t p2r = OS_INVALID_HANDLE, p2w = OS_INVALID_HANDLE;
    os_process_t proc1, proc2;
    int started1 = 0, started2 = 0;
    char child_path[OS_PATH_MAX];
    int rc = 0;

    if (os_sibling_path("child", child_path, sizeof(child_path)) < 0)
        return 1;

    name1 = ask_name("Файл для child1: ");
    if (name1 == NULL)
        return 1;
    name2 = ask_name("Файл для child2: ");
    if (name2 == NULL)
    {
        free(name1);
        return 1;
    }

    if (os_file_open_write(name1, &file1) < 0 || os_file_open_write(name2, &file2) < 0 ||
        os_pipe_create(&p1r, &p1w) < 0 || os_pipe_create(&p2r, &p2w) < 0)
    {
        rc = 1;
        goto cleanup;
    }

    if (os_process_start(child_path, p1r, file1, &proc1) < 0)
    {
        rc = 1;
        goto cleanup;
    }
    started1 = 1;
    close_h(&p1r);
    close_h(&file1);

    if (os_process_start(child_path, p2r, file2, &proc2) < 0)
    {
        rc = 1;
        goto cleanup;
    }
    started2 = 1;
    close_h(&p2r);
    close_h(&file2);

    {
        char *line;
        size_t len;
        int err = 0;
        unsigned long idx = 0;

        while ((line = read_line(stdin, &len, 1, &err)) != NULL)
        {
            os_handle_t h = (++idx % 2 == 1) ? p1w : p2w; /* нечёт -> pipe1 */

            if (os_write_all(h, line, len) < 0 ||
                (line[len - 1] != '\n' && os_write_all(h, "\n", 1) < 0))
            {
                fprintf(stderr, "parent: ошибка записи в pipe%d\n", idx % 2 == 1 ? 1 : 2);
                free(line);
                rc = 1;
                break;
            }
            free(line);
        }
        if (err)
            rc = 1;
    }

cleanup:
    close_h(&p1w);
    close_h(&p2w);
    close_h(&p1r);
    close_h(&p2r);
    close_h(&file1);
    close_h(&file2);

    if (started1 && os_process_wait(&proc1, "child1") < 0)
        rc = 1;
    if (started2 && os_process_wait(&proc2, "child2") < 0)
        rc = 1;

    free(name1);
    free(name2);
    return rc;
}
