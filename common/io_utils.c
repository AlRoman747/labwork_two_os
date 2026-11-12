#include "io_utils.h"

#include <stdlib.h>

#include "errors.h"

char *read_line(FILE *f, size_t *len, int keep_nl, int *err)
{
    size_t cap = 128, n = 0;
    char *buf = (char *)malloc(cap);
    int c;

    *err = 0;
    *len = 0;
    if (buf == NULL) {
        err_errno("read_line: malloc");
        *err = 1;
        return NULL;
    }

    while ((c = fgetc(f)) != EOF) {
        if (n + 2 >= cap) {
            char *tmp;
            cap *= 2;
            tmp = (char *)realloc(buf, cap);
            if (tmp == NULL) {
                err_errno("read_line: realloc");
                free(buf);
                *err = 1;
                return NULL;
            }
            buf = tmp;
        }
        if (c == '\n') {
            if (keep_nl)
                buf[n++] = '\n';
            else if (n > 0 && buf[n - 1] == '\r')
                n--;
            break;
        }
        buf[n++] = (char)c;
    }

    if (c == EOF && n == 0) {
        if (ferror(f)) {
            err_errno("read_line: чтение");
            *err = 1;
        }
        free(buf);
        return NULL;
    }
    buf[n] = '\0';
    *len = n;
    return buf;
}
