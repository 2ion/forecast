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

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barplot.h"
#include "cache.h"
#include "configfile.h"
#include "forecast.h"
#include "hash.h"
#include "network.h"
#include "render.h"
#include "units.h"

#define FREE_IF(flag, var) if(flag == true) free((void*)(var))

/* globals */

#define CLI_OPTIONS "dehL:l:m:rs:u:v"
static const char *options = CLI_OPTIONS;
static const struct option options_long[] = {
  { "dump",             no_argument,        NULL, 'd' },
  { "extend-hourly",    no_argument,        NULL, 'e' },
  { "help",             no_argument,        NULL, 'h' },
  { "language",         required_argument,  NULL, 'X' },
  { "location",         required_argument,  NULL, 'l' },
  { "location-by-name", required_argument,  NULL, 'L' },
  { "mode",             required_argument,  NULL, 'm' },
  { "request",          no_argument,        NULL, 'r' },
  { "step",             required_argument,  NULL, 's' },
  { "units",            required_argument,  NULL, 'u' },
  { "version",          no_argument,        NULL, 'v' },
  { 0,                  0,                  0,    0   }
};

static int    parse_location(const char *s, double *la, double *lo);
static int    lookup_location(Config *c, const char *n);
static void   store_location_name(Config *c, const char *name);
static void   list_locations(const Config *c);
static void   usage(void);
static int    parse_integer(const char*);
static int    process_cmdline(int, char**, Config*);

void store_location_name(Config *c, const char *name)
{
  if(c->location.name)
    free(c->location.name);
  if((c->location.name = strdup(name)) == NULL)
    LERROR(EXIT_FAILURE, errno, "strdup()");
}

int parse_integer(const char *s) {
  long int i = strtol(s, NULL, 0xA);
  if(    (i == LONG_MIN && errno == ERANGE)
      || (i == LONG_MAX && errno == ERANGE))
    return -1;
  return i;
}

void hash_location(Config *c) {
  char *b;
  size_t blen = snprintf(NULL, 0, "%f%f", c->location.latitude, c->location.longitude);
  b = malloc(blen);
  GUARD_MALLOC(b);
  snprintf(b, 0, "%f%f", c->location.latitude, c->location.longitude);
  if(md5str((const char*)b, &c->location.hash[0], sizeof(c->location.hash)) != 0)
    LERROR(EXIT_FAILURE, 0, "Failed to hash location");
  free(b);
}

int lookup_location(Config *c, const char *n) {
  for(size_t i = 0; i < c->location_map_len; i++) {
    if(strcasecmp((const char*)c->location_map[i].name, n) == 0) {
      c->location.latitude = c->location_map[i].latitude;
      c->location.longitude = c->location_map[i].longitude;
      return i;
    }
  }
  return -1;
}

void list_locations(const Config *c) {
  for(size_t i = 0; i < c->location_map_len; i++) {
    if(i == 0) printf("Locations: ");
    if(i < c->location_map_len-1)
      printf("%s, ", c->location_map[i].name);
    else
      puts(c->location_map[i].name);
  }
}

int parse_location(const char *s, double *la, double *lo) {
  char *buf, *col, *e;

  if((col = strchr(s, ':')) == NULL)
    return -1;

  buf = malloc(col - s + 1);
  GUARD_MALLOC(buf);

  memcpy(buf, s, col - s + 1);
  buf[col-s] = '\0';
  *la = strtod(buf, NULL);
  e = (char*) s;
  while(*e++);
  buf = realloc(buf, e - col);
  GUARD_MALLOC(buf);
  memcpy(buf, col + 1, e - col);
  buf[e-col-1] = '\0';
  *lo = strtod(buf, NULL);
  free(buf);

  return 0;
}

void usage(void) {
  puts("Usage:\n"
       "  forecast [" CLI_OPTIONS "] [LONGFORM OPTIONS]\n"
       "Options:\n"
       "  -d|--dump                      Dump the JSON data and a newline to stdout\n"
       "  -e|--extend-hourly             Request data for one week instead of two days\n"
       "                                 for hourly forecasts.\n"
       "  -h|--help                      Print this message and exit\n"
       "  -L|--location-by-name   NAME   Select a location predefined in the configuration file\n"
       "  -l|--location           CHOORD Query the weather at this location; CHOORD is a string in the format\n"
       "                                 <latitude>:<longitude> where the choordinates are given as floating\n"
       "                                 point numbers\n"
       "  --language              NAME   Set the language for verbal descriptions. One of ar, bs, de, el, en, es,\n"
       "                                 fr, hr, it, nl, pl, pt, ru, sk, sv, tet, tr, uk, x-pig-latin, zh, zh-tw.\n"
       "  -m|--mode               MODE   One of print, print-hourly, plot-hourly, plot-daily, plot-precip-daily,\n"
       "                                 plot-precip-hourly, plot-daylight. Defaults to 'print'\n"
       "  -r|--request                   Bypass the cache if a cache file exists\n"
       "  -s|--step               N      In hourly plots, use only every Nth datapoint.\n"
       "  -u|--units              UNITS  Location-specific unit table to be used. One of si, us, uk, ca, auto.\n"
       "                                 When specifying 'auto', the unit will be set depending on location.\n"
       "  -v|--version                   Print program version and exit"
       );
}

int process_cmdline(int argc, char**argv, Config *c) {
  int opt;

  while((opt = getopt_long(argc, argv, options, options_long, NULL)) != -1) {
    switch(opt) {
      case 'h':
        usage();
        return FORECAST_SUCCESS;

      case 'L':
        if((c->location_map_idx = lookup_location(c, (const char*)optarg)) == -1)
          printf("-L: location <%s> not defined in config file, ignoring\n", optarg);
        else
          store_location_name(c, optarg);
        break;

      case 'l':
        if(parse_location((const char*)optarg, &(c->location.latitude), &(c->location.longitude)) == -1)
          puts("-l: malformed option argument");
        else
          store_location_name(c, optarg);
        c->bypass_cache = true;
        break;

      case 'v':
        puts(PACKAGE_STRING);
        puts("Compiled on: " __DATE__ " " __TIME__);
        printf("Configuration file: %s\n", c->path);
        list_locations(c);
        return FORECAST_SUCCESS;

      case '?':
        usage();
        return FORECAST_ERROR;

      case 'm':
        if((c->op = match_mode_arg((const char*)optarg)) == -1) {
          puts("-m: invalid mode, defaulting to 'print'");
          c->op = OP_PRINT_CURRENTLY;
        }
        break;

      case 'd':
        c->dump_data = true;
        break;

      case 'r':
        c->bypass_cache = true;
        break;

      case 'e':
        if(!c->extend_hourly) /* when overriding the config file */
          c->bypass_cache = true;
        c->extend_hourly = true;
        break;

      case 'u':
        if((c->units = match_units_arg((const char*)optarg)) == -1) {
          puts("-u: invalid unit table, defaulting to 'si'");
          c->units = UNITS_SI;
        }
        break;

      case 's':
        if((c->plot.hourly.step = parse_integer((const char*)optarg)) == -1) {
          puts("-s: integer over- or underflow, defaulting to 1");
            c->plot.hourly.step = 1;
        }
        if((c->extend_hourly && c->plot.hourly.step > 168) ||
            (!c->extend_hourly && c->plot.hourly.step > 48))
          puts("-s: warning: step length is greater than the available data set");
        break;

      case 'X':
        {
          int deflang = c->language;
          if((c->language = match_lang_arg((const char*)optarg)) == -1) {
            puts("--language: invalid identifier, defaulting to 'en'");
            c->language = LANG_EN;
          }
          if(c->language != deflang) /* when the requested language is different from the default language */
            c->bypass_cache = true;
        }
        break;
    }
  }

  return FORECAST_PROCEED;
}

int main(int argc, char **argv) {
  Config c = CONFIG_NULL;
  Data d = {0};

  set_config_path(&c);
  if(load_config(&c) != 0)
    LERROR(EXIT_FAILURE, 0, "Failed to load the configuration file");

  switch(process_cmdline(argc, argv, &c)) {
    case FORECAST_SUCCESS:
      return EXIT_SUCCESS;
    case FORECAST_ERROR:
      return EXIT_FAILURE;
    default:
      break;
  }

  if(strlen(c.apikey) == 0)
    LERROR(EXIT_FAILURE, 0, "API key must not be empty.");

  if(string_isalnum(c.apikey) == -1)
    LERROR(EXIT_FAILURE, 0, "API key is not a hexstring: %s", c.apikey);

  if(c.bypass_cache == true || cache_load(&c, &d) == -1) {
    if(request(&c, &d) != 0)
      puts("Failed to request data");
    else
      cache_save(&c, &d);
  }

  if(c.dump_data) {
    write(STDOUT_FILENO, d.data, d.datalen);
    putchar('\n');
  } else {
    barplot_start(&c.plot);
    render(&c, &d);
    barplot_end();
  }

  if(d.data != NULL)
    free(d.data);

  free_config(&c);

  return EXIT_SUCCESS;
}
