#ifndef TREE_H
#define TREE_H

#include <json-c/json.h>
#include <json-c/json_object.h>
#include <string.h>
#include <talloc.h>
#include <stdio.h>
#include "forecast.h"

struct _TLocation;
struct _TData;

typedef struct _TLocation TLocation;
typedef struct _TData TData;

enum { TD_DOUBLE, TD_STRING };

struct _TData
{
  char *name;
  int type;
  union
  {
    double d;
    char *s;
  } value;
};

struct _TLocation
{
  /* not provided by the JSON API */
  char *name;

  char *timezone;
  char *units;
  double latitude;
  double longitude;
  int offset;

  char *w_daily_summary;
  char *w_hourly_summary;

  size_t w_currently_len;
  size_t w_daily_len;
  size_t w_hourly_len;

  size_t *w_daily_chld_len;
  size_t *w_hourly_chld_len;

  TData ***w_daily;
  TData ***w_hourly;
  TData **w_currently;
};

TLocation*  tree_new(const Data*);
char*       tree_char(TData**, size_t, const char*);
double*     tree_double(TData**, size_t, const char*);
int*        tree_int(TData**, size_t, const char*);
void        tree_free(TLocation*);
void        tree_print(TLocation*, FILE*);
int         tree_typeof(const char*);

#endif
