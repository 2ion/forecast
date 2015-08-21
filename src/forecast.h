/*
 *  forecast - query weather forecasts from forecast.io
 *  Copyright (C) 2015 Jens John <dev@2ion.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FORECAST_H
#define FORECAST_H

#include <sys/types.h>

#include <errno.h>
#include <error.h>

#include "barplot.h"
#include "config.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)
#define GUARD_MALLOC(v) \
  if((v) == NULL)       \
    LERROR(EXIT_FAILURE, errno, "malloc(%s) returned NULL", #v);

/* types */

enum {
  OP_PLOT_HOURLY,
  OP_PLOT_DAILY,
  OP_PRINT_CURRENTLY,
  OP_PRINT_HOURLY,
  OP_PLOT_PRECIPITATION_HOURLY,
  OP_PLOT_PRECIPITATION_DAILY,
  OP_PLOT_DAYLIGHT
};

typedef struct {
  char *name;
  double latitude;
  double longitude;
} Location;

typedef struct {
  char *path;
  const char *apikey;
  char *cache_file;
  Location location;
  Location *location_map;
  size_t location_map_len;
  PlotCfg plot;
  int op;
  int max_cache_age;
  bool bypass_cache;
} Config;

#define CONFIG_NULL         \
{                           \
  .path = NULL,             \
  .apikey = NULL,           \
  .cache_file = NULL,       \
  .max_cache_age = 0,       \
  .location = {             \
    .latitude = 0.0,        \
    .longitude = 0.0,       \
    .name = NULL            \
  },                        \
  .location_map = NULL,     \
  .location_map_len = 0,    \
  .plot = PLOTCFG_DEFAULT,  \
  .op = OP_PRINT_CURRENTLY, \
  .bypass_cache = false     \
}

typedef struct {
  char *data;
  size_t datalen;
} Data;

#define DATA_NULL           \
{                           \
  .data = NULL,             \
  .datalen = 0              \
}

#endif
