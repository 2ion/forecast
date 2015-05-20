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
#define LOOKUP_STRING(key)                                    \
  if(config_lookup_string(&cfg, #key, &tmp) == CONFIG_TRUE) { \
    c->key = malloc(strlen(tmp)+1);                           \
    GUARD_MALLOC(c->key);                                     \
    memcpy(c->key, tmp, strlen(tmp)+1);                       \
  } else {                                                    \
    LOOKUP_LERROR(key)                                        \
    goto return_error;                                        \
  }

  /* General */

  LOOKUP_INT(max_cache_age)
  LOOKUP_FLOAT(location.longitude)
  LOOKUP_FLOAT(location.latitude)

  if(config_lookup_string(&cfg, "op", &tmp) != CONFIG_TRUE) {
    LOOKUP_LERROR(op);
    goto return_error;
  } else {
    c->op = match_mode_arg(tmp);
    if(c->op == -1)
      goto return_error;
  }

  LOOKUP_STRING(cache_file)

  /* Plot */

  LOOKUP_INT(plot.height)
  LOOKUP_INT(plot.bar.width)
  LOOKUP_INT(plot.hourly.succeeding_hours)
  LOOKUP_INT(plot.daylight.width_max)
  LOOKUP_FLOAT(plot.daylight.width_frac)

  LOOKUP_STRING(plot.daily.label_format)
  LOOKUP_STRING(plot.hourly.label_format)
  LOOKUP_STRING(plot.daylight.date_label_format);
  LOOKUP_STRING(plot.daylight.time_label_format);

  if(config_lookup_string(&cfg, "plot.bar.color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.bar.color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.bar.color)
  }

  if(config_lookup_string(&cfg, "plot.bar.overlay_color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.bar.overlay_color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.bar.overlay_color)
  }

  if(config_lookup_string(&cfg, "plot.legend.texthighlight_color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.legend.texthighlight_color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.legend.texthighlight_color)
  }

  if(config_lookup_string(&cfg, "plot.legend.color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.legend.color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.legend.color)
  }

  if(config_lookup_string(&cfg, "plot.precipitation.bar_color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.precipitation.bar_color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.precipitation.bar_color)
  }

  if(config_lookup_string(&cfg, "plot.daylight.color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.daylight.color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.daylight.color)
  }


#undef LOOKUP_INT
#undef LOOKUP_FLOAT
#undef LOOKUP_STRING
#undef LOOKUP_GENERIC

  config_destroy(&cfg);
  return 0;

return_error:
  config_destroy(&cfg);
  return -1;
}

void free_config(Config *c) {
#define FREE_KEY(key) \
  if((key) != NULL) free(key)
  FREE_KEY(c->path);
  FREE_KEY(c->plot.daily.label_format);
  FREE_KEY(c->plot.hourly.label_format);
  FREE_KEY((void*)c->apikey);
  FREE_KEY((void*)c->cache_file);
#undef FREE_KEY
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
  for(int i = 0; i < strlen(s); i++)
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
