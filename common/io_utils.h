#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stddef.h>
#include <stdio.h>

/*
 * Читает одну строку произвольной длины из f.
 * keep_nl != 0: символ '\n' сохраняется в конце строки;
 * keep_nl == 0: '\n' (и предшествующий '\r') отбрасывается.
 * Возвращает malloc-буфер (с завершающим '\0'), длина - в *len.
 * NULL: конец ввода (*err == 0) или ошибка (*err == 1).
 * Вызывающий освобождает результат через free().
 */
char *read_line(FILE *f, size_t *len, int keep_nl, int *err);

#endif
