#ifndef TREE_H
#define TREE_H

#include <string.h>
#include <talloc.h>
#include "forecast.h"

struct _TLocation;
struct _TData;

typedef struct _TLocation TLocation;
typedef struct _TData TData;

struct _TData
{
  char *name;
  union
  {
    int i;
    double d;
    char *s;
  } value;
};

struct _TLocation
{
  char *name;
  double latitude;
  double longitude;
  char *timezone;
  int offset;


  TData **w_currently;
  size_t w_currently_len;

  TData **w_hourly;
  size_t w_hourly_len;
  char *w_hourly_summary;

  TData **w_daily;
  size_t w_daily_len;
  char *w_daily_summary;
};

TLocation* tree_new(const Data*);
void tree_free(TLocation*);
int* tree_int(TData**, size_t, const char*);
char* tree_char(TData**, size_t, const char*);
double* tree_float(TData**, size_t, const char*);

#endif
