/*
 *  forecast - query weather forecasts from forecast.io
 *  Copyright (C) 2015 Jens John <dev@2ion.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
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
#include <stdbool.h>
#include <stdlib.h>

#include "barplot.h"
#include "config.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

#define _PASTE(x, y) x ## _ ## y

#define PASTE(x, y) _PASTE(x, y)

#define NAME(prefix, name) PASTE(prefix, name)

#define EXTRACT(object, key)  \
  struct json_object *(key);  \
  json_object_object_get_ex((object), #key, &(key));

#define EXTRACT_PREFIXED(object, key)                                                         \
  struct json_object *(NAME(object, key)) = NULL;                                             \
  if(json_object_object_get_ex((object), #key, &(NAME(object, key))) != TRUE)                 \
    LERROR(EXIT_FAILURE, 0, "json_object_object_get_ex() failed: %s", #key);

#define RENDER_BEARING(deg) \
  deg ==  0.00 ? "N" : (deg <  45.0 ? "NNE" : (deg ==  45.0 ? "NE" : (deg <  90.0 ? "ENE" : \
( deg ==  90.0 ? "E" : (deg < 135.0 ? "ESE" : (deg == 135.0 ? "SE" : (deg < 180.0 ? "SSE" : \
( deg == 180.0 ? "S" : (deg < 225.0 ? "SSW" : (deg == 225.0 ? "SW" : (deg < 270.0 ? "WSW" : \
( deg == 270.0 ? "W" : (deg < 315.0 ? "WNW" : (deg == 315.0 ? "NW" : "NNW"))))))))))))))

#define CHECKCOLOR(var, name) if(strcmp(tmp, #name) == 0) (var) = PASTE(COLOR, name);
#define CHECKCOLORS(var)             \
    CHECKCOLOR(var, BLACK)           \
    else CHECKCOLOR(var, RED)        \
    else CHECKCOLOR(var, GREEN)      \
    else CHECKCOLOR(var, YELLOW)     \
    else CHECKCOLOR(var, BLUE)       \
    else CHECKCOLOR(var, MAGENTA)    \
    else CHECKCOLOR(var, CYAN)       \
    else CHECKCOLOR(var, WHITE)

/* types */

enum {
  OP_PLOT_HOURLY,
  OP_PLOT_DAILY,
  OP_PRINT_CURRENTLY,
  OP_PRINT_HOURLY,
  OP_PLOT_PRECIPITATION_HOURLY,
  OP_PLOT_PRECIPITATION_DAILY
};

typedef struct {
  char *path;
  const char *apikey;
  char *cache_file;
  struct {
    double latitude;
    double longitude;
  } location;
  PlotCfg plot;
  int op;
  int max_cache_age;
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
  },                        \
  .plot = PLOTCFG_DEFAULT,  \
  .op = OP_PRINT_CURRENTLY  \
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
