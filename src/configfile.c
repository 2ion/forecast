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

#include "configfile.h"

static void load_location_map(config_t *cfg, Config *c);

int load_config(Config *c) {
  assert(c);

  config_t cfg;
  const char *apikey;
  const char *tmp;

  if(access(c->path, R_OK) != 0) {
    LERROR(0, errno, "%s", c->path);
    return -1;
  }

  config_init(&cfg);

  if(config_read_file(&cfg, c->path) != CONFIG_TRUE) {
    LERROR(0, 0, "[%s, %d] %s",
        config_error_file(&cfg),
        config_error_line(&cfg),
        config_error_text(&cfg));
    goto return_error;
  }

  if(config_lookup_string(&cfg, "apikey", &apikey) != CONFIG_TRUE) {
    LERROR(0, 0, "No API key found.");
    goto return_error;
  }
  c->apikey = malloc(strlen(apikey) + 1);
  GUARD_MALLOC(c->apikey);
  memcpy((void*)c->apikey, apikey, strlen(apikey) + 1);

#define LOOKUP_LERROR(key) LERROR(0, 0, "[" #key "] not configured");
#define LOOKUP_GENERIC(func, key)                   \
  if(func(&cfg, #key, &(c->key)) != CONFIG_TRUE) {  \
    LOOKUP_LERROR(key)                              \
    goto return_error;                              \
  }
#define LOOKUP_INT(key) LOOKUP_GENERIC(config_lookup_int, key)
#define LOOKUP_FLOAT(key) LOOKUP_GENERIC(config_lookup_float, key)
#define LOOKUP_BOOL(key) LOOKUP_GENERIC(config_lookup_bool, key)
#define LOOKUP_STRING(key)                                    \
  if(config_lookup_string(&cfg, #key, &tmp) == CONFIG_TRUE) { \
    c->key = malloc(strlen(tmp)+1);                           \
    GUARD_MALLOC(c->key);                                     \
    memcpy(c->key, tmp, strlen(tmp)+1);                       \
  } else {                                                    \
    LOOKUP_LERROR(key)                                        \
    goto return_error;                                        \
  }
#define LOOKUP_COLOR(key)                                     \
  if(config_lookup_string(&cfg, #key, &tmp) == CONFIG_TRUE) { \
    CHECKCOLORS(c->key)                                       \
  } else {                                                    \
    LOOKUP_LERROR(key)                                        \
    goto return_error;                                        \
  }

  /* General */

  LOOKUP_BOOL(extend_hourly);

  /* language */
  if(config_lookup_string(&cfg, "language", &tmp) == CONFIG_TRUE) {
    c->language = match_lang_arg(tmp);
    if(c->language == -1)
      goto return_error;
  }

  /* units */
  if(config_lookup_string(&cfg, "units", &tmp) == CONFIG_TRUE) {
    c->units = match_units_arg(tmp);
    if(c->units == -1)
      goto return_error;
  }

  /* Locations */
  LOOKUP_FLOAT(location.latitude);
  LOOKUP_FLOAT(location.longitude);
  load_location_map(&cfg, c);

  LOOKUP_INT(max_cache_age);

  LOOKUP_STRING(cache_file);
  /* Check if cache_file contains a %s escape */
  if(strstr(c->cache_file, "%s") == NULL) {
    puts("[cache_file] must include a '%s' escape sequence");
    goto return_error;
  }

  if(config_lookup_string(&cfg, "op", &tmp) != CONFIG_TRUE) {
    LOOKUP_LERROR(op);
    goto return_error;
  } else {
    c->op = match_mode_arg(tmp);
    if(c->op == -1)
      goto return_error;
  }

  /* Plot */

  LOOKUP_COLOR(plot.bar.color);
  LOOKUP_COLOR(plot.bar.overlay_color);
  LOOKUP_COLOR(plot.daylight.color);
  LOOKUP_COLOR(plot.legend.color);
  LOOKUP_COLOR(plot.legend.texthighlight_color);
  LOOKUP_COLOR(plot.precipitation.bar_color);

  LOOKUP_FLOAT(plot.daylight.width_frac);

  LOOKUP_INT(plot.bar.width);
  LOOKUP_INT(plot.daylight.width_max);
  LOOKUP_INT(plot.height);
  LOOKUP_INT(plot.hourly.step);
  LOOKUP_INT(plot.hourly.succeeding_hours);

  LOOKUP_STRING(plot.daily.label_format);
  LOOKUP_STRING(plot.daylight.date_label_format);
  LOOKUP_STRING(plot.daylight.time_label_format);
  LOOKUP_STRING(plot.hourly.label_format);

#undef LOOKUP_BOOL
#undef LOOKUP_COLOR
#undef LOOKUP_INT
#undef LOOKUP_FLOAT
#undef LOOKUP_STRING
#undef LOOKUP_GENERIC
#undef LOOKUP_LERROR

  config_destroy(&cfg);
  return 0;

return_error:
  config_destroy(&cfg);
  return -1;
}

void load_location_map(config_t *cfg, Config *c) {
  assert(cfg);
  assert(c);
  const config_setting_t *map;

  map = config_lookup(cfg, "location_map");
  if(map == NULL) return;
  if(config_setting_is_group(map) != CONFIG_TRUE)
    return;

  for(int i = 0; i < config_setting_length(map); i++) {
    const config_setting_t *array = config_setting_get_elem(map, (unsigned)i);
    const char *n = config_setting_name(array);
    if(config_setting_length(array) < 2) continue; /* accept only arrays of length 2 */
    double la = config_setting_get_float_elem(array, 0);
    double lo = config_setting_get_float_elem(array, 1);
    if(c->location_map_len == 0) {
      c->location_map_len = 1;
      c->location_map = malloc(sizeof(Location));
    } else {
      c->location_map_len += 1;
      c->location_map = realloc(c->location_map, c->location_map_len*sizeof(Location));
    }
    GUARD_MALLOC(c->location_map);
    c->location_map[i].latitude = la;
    c->location_map[i].longitude = lo;
    c->location_map[i].name = malloc(strlen(n)+1);
    GUARD_MALLOC(c->location_map[i].name);
    memcpy(c->location_map[i].name, n, strlen(n)+1);
  }
}

void free_config(Config *c) {
#define FREE_KEY(key) \
  if((key) != NULL) free(key)
  FREE_KEY(c->path);
  FREE_KEY(c->plot.daily.label_format);
  FREE_KEY(c->plot.hourly.label_format);
  FREE_KEY((void*)c->apikey);
  FREE_KEY((void*)c->cache_file);
  FREE_KEY(c->plot.daylight.time_label_format);
  FREE_KEY(c->plot.daylight.date_label_format);
#undef FREE_KEY
  if(c->location_map_len > 0) {
    for(size_t i = 0; i < c->location_map_len; i++)
      free(c->location_map[i].name);
    free(c->location_map);
  }
}


int match_mode_arg(const char *str) {
  if(strcmp(str, "plot-hourly") == 0)
    return OP_PLOT_HOURLY;
  else if(strcmp(str, "print") == 0)
    return OP_PRINT_CURRENTLY;
  else if(strcmp(str, "print-hourly") == 0)
    return OP_PRINT_HOURLY;
  else if(strcmp(str, "plot-daily") == 0)
    return OP_PLOT_DAILY;
  else if(strcmp(str, "plot-precip-daily") == 0)
    return OP_PLOT_PRECIPITATION_DAILY;
  else if(strcmp(str, "plot-precip-hourly") == 0)
    return OP_PLOT_PRECIPITATION_HOURLY;
  else if(strcmp(str, "plot-daylight") == 0)
    return OP_PLOT_DAYLIGHT;
  else
    return -1;
}

int string_isalnum(const char *s) {
  for(size_t i = 0; i < strlen(s); i++)
    if(isalnum(s[i]) == 0)
      return -1;
  return 0;
}

void set_config_path(Config *c) {
  int plen;

  if(getenv("FORECAST_CONFIG_PATH") == NULL) {
    plen = snprintf(NULL, 0, "%s/%s", getenv("HOME"), RCNAME) + 1;
    c->path = malloc(plen);
    GUARD_MALLOC(c->path);
    snprintf((char*)c->path, plen, "%s/%s", getenv("HOME"), RCNAME);
  } else {
    char *p = getenv("FORECAST_CONFIG_PATH");
    plen = strlen(p)+1;
    c->path = malloc(plen);
    GUARD_MALLOC(c->path);
    memcpy(c->path, p, plen);
  }

}
