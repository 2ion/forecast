#include "lerror.h"

static void vlerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, va_list ap);
static inline TALLOC_CTX* allocate_format(TALLOC_CTX *parent, size_t len);

/*********************************************************************/

void lerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  vlerror(file_name, func_name, file_line, status, errnum, stream, msg, arg);
  va_end(arg);
}

/*********************************************************************/

void vlerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, va_list ap)
{
  char *format;
  size_t formatlen;

  TALLOC_CTX *tc = talloc_new(NULL);

  if(errnum != 0)
  {
    const char *errstr = strerror(errnum);
    formatlen = snprintf(NULL, 0, "%s:%s:%u: %s: %s\n", file_name,
        func_name, file_line, errstr, msg) + 1;
    format = allocate_format(tc, formatlen);
    snprintf(format, formatlen, "%s:%s:%u: %s: %s\n", file_name,
        func_name, file_line, errstr, msg);
  }
  else
  {
    formatlen = snprintf(NULL, 0, "%s:%s:%u: %s\n", file_name, func_name,
        file_line, msg) + 1;
    format = allocate_format(tc, formatlen);
    snprintf(format, formatlen, "%s:%s:%u: %s\n", file_name, func_name,
        file_line, msg);
  }

  vfprintf(stream, format, ap);

  talloc_free(tc);

  if(status != EXIT_SUCCESS)
    exit(status);
}

TALLOC_CTX* allocate_format(TALLOC_CTX *parent, size_t len)
{
  TALLOC_CTX *t = talloc_array(parent, char, len);
  if(t == NULL)
  {
    fprintf(stderr, "lerror:allocate_format:memory allocation error\n");
    exit(EXIT_FAILURE);
  }
  return t;
}

