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

#include "barplot.h"
#include "language.h"
#include "lerror.h"
#include "data.h"

#include "config.h"

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
  char hash[33];
  double latitude;
  double longitude;
} Location;

typedef struct {
  char *path;
  const char *apikey;
  char *cache_file;
  Location location;
  Location *location_map;
  int location_map_len;
  int location_map_idx;
  PlotCfg plot;
  int op;
  int max_cache_age;
  bool bypass_cache;
  int extend_hourly; /* 'int' because if 'bool', libconfig complains (I'm just too lazy) */
  int units;
  int language;
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
    .name = NULL \
  },                        \
  .location_map = NULL,     \
  .location_map_len = 0,    \
  .location_map_idx = -1,   \
  .plot = PLOTCFG_DEFAULT,  \
  .op = OP_PRINT_CURRENTLY, \
  .bypass_cache = false,    \
  .extend_hourly = false,   \
  .units = UNITS_AUTO,      \
  .language = LANG_EN       \
}

/* construct concatenated symbol names */
#define _PASTE(x, y) x ## _ ## y
#define PASTE(x, y) _PASTE(x, y)

#endif
