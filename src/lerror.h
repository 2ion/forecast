#ifndef LERROR_H
#define LERROR_H

/*
 * a portable re-implementation of GNU libc's error_at_line(), providing
 * LERROR(). This should build against OSX's libc -- to be tested.
 */

#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <talloc.h>

void lerror(const char *file_name, const char *func_name, unsigned int file_line, int status,
    int errnum, FILE *stream, const char *msg, ...);

#define LERROR(status, errnum, ...) lerror((__FILE__), (__func__), (__LINE__), \
    (status), (errnum), stderr, __VA_ARGS__)

#endif
