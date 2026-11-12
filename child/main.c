/*
 * child - читает строки из stdin, инвертирует каждую и пишет в stdout.
 * stdin/stdout перенаправлены родителем (pipe -> файл).
 */
#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "io_utils.h"

static void reverse(char *s, size_t len)
{
    size_t i, j;
    char tmp;

    if (len < 2)
        return;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
}

int main(void)
{
    char *line;
    size_t len;
    int err = 0;

    while ((line = read_line(stdin, &len, 0, &err)) != NULL) {
        reverse(line, len);
        if (fwrite(line, 1, len, stdout) != len || fputc('\n', stdout) == EOF ||
            fflush(stdout) != 0) {
            err_errno("child: запись в stdout");
            free(line);
            return 1;
        }
        free(line);
    }
    return err ? 1 : 0;
}
