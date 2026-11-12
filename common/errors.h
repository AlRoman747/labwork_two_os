#ifndef ERRORS_H
#define ERRORS_H

/* Печатает в stderr "what: <strerror(errno)>". */
void err_errno(const char *what);

/* Системная ошибка платформы: GetLastError (Windows) или errno (Linux). */
void err_sys(const char *what);

#endif
