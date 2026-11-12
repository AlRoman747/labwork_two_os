#include "errors.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

void err_errno(const char *what)
{
    fprintf(stderr, "%s: %s\n", what, strerror(errno));
}

void err_sys(const char *what)
{
#ifdef _WIN32
    DWORD code = GetLastError();
    char *msg = NULL;
    size_t n;

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, code, 0, (LPSTR)&msg, 0, NULL);
    if (msg != NULL) {
        n = strlen(msg);
        while (n > 0 && (msg[n - 1] == '\r' || msg[n - 1] == '\n' || msg[n - 1] == ' '))
            msg[--n] = '\0';
    }
    fprintf(stderr, "%s: [%lu] %s\n", what, (unsigned long)code, msg ? msg : "");
    LocalFree(msg);
#else
    err_errno(what);
#endif
}
