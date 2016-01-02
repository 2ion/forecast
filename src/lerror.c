#include "lerror.h"

static void vlerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, va_list ap);

/*********************************************************************/

void lerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  vlerror(file_name, func_name, file_line, status, errnum, stream, msg, arg);
  va_end(arg);
}

void vlerror(const char *file_name, const char *func_name, unsigned int file_line,
    int status, int errnum, FILE *stream, const char *msg, va_list ap)
{
  TALLOC_CTX *tc = talloc_new(NULL);
  char *format;
  size_t formatlen;

  if(errnum != 0)
  {
    const char *errstr = strerror(errnum);
    formatlen = snprintf(NULL, 0, "%s:%s:%u: %s: %s\n", file_name, func_name, file_line,
        errstr, msg) + 1;
    format = talloc_array(tc, char, formatlen);
    snprintf(format, formatlen, "%s:%s:%u: %s: %s\n", file_name, func_name, file_line,
        errstr, msg);
  }
  else
  {
    formatlen = snprintf(NULL, 0, "%s:%s:%u: %s\n", file_name, func_name, file_line, msg) + 1;
    format = talloc_array(tc, char, formatlen);
    snprintf(format, formatlen, "%s:%s:%u: %s\n", file_name, func_name, file_line, msg);
  }

  vfprintf(stream, format, ap);

  talloc_free(tc);

  if(status != EXIT_SUCCESS)
    exit(status);
}

