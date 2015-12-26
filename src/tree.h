#ifndef TREE_H
#define TREE_H

#include <json-c/json.h>
#include <json-c/json_object.h>
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
  /* not provided by the JSON API */
  char *name;

  double latitude;
  double longitude;
  char *timezone;
  int offset;

  char *w_hourly_summary;
  char *w_daily_summary;

  size_t w_currently_len;
  size_t w_daily_len;
  size_t w_hourly_len;

  size_t *w_daily_chld_len;
  size_t *w_hourly_chld_len;

  TData **w_currently;
  TData ***w_daily;
  TData ***w_hourly;
};

TLocation* tree_new(const Data*);
void tree_free(TLocation*);
int* tree_int(TData**, size_t, const char*);
char* tree_char(TData**, size_t, const char*);
double* tree_float(TData**, size_t, const char*);

#endif
