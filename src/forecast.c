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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barplot.h"
#include "cache.h"
#include "configfile.h"
#include "forecast.h"
#include "network.h"
#include "render.h"

#define FREE_IF(flag, var) if(flag == true) free((void*)(var))

/* globals */

#define CLI_OPTIONS "c:dhl:m:rv"
static const char *options = CLI_OPTIONS;
static const struct option options_long[] = {
  { "help",     no_argument,        NULL, 'h' },
  { "location", required_argument,  NULL, 'l' },
  { "config",   required_argument,  NULL, 'c' },
  { "version",  no_argument,        NULL, 'v' },
  { "mode",     required_argument,  NULL, 'm' },
  { "dump",     no_argument,        NULL, 'd' },
  { "request",  no_argument,        NULL, 'r' },
  { 0,          0,                  0,    0   }
};

static int    parse_location(const char *s, double *la, double *lo);
static void   usage(void);

int parse_location(const char *s, double *la, double *lo) {
  char *buf, *col, *e;

  if((col = strchr(s, ':')) == NULL)
    return -1;

  buf = malloc(col - s + 1);
  memcpy(buf, s, col - s + 1);
  buf[col-s] = '\0';
  *la = strtod(buf, NULL);
  e = (char*) s;
  while(*e++);
  buf = realloc(buf, e - col);
  memcpy(buf, col + 1, e - col);
  buf[e-col-1] = '\0';
  *lo = strtod(buf, NULL);
  free(buf);

  return 0;
}

void usage(void) {
  puts("Usage:\n"
       "  forecast [" CLI_OPTIONS "] [OPTIONS]\n"
       "Options:\n"
       "  -c|--config    PATH   Configuration file to use\n"
       "  -d|--dump             Dump the JSON data and a newline to stdout\n"
       "  -h|--help             Print this message and exit\n"
       "  -l|--location  CHOORD Query the weather at this location; CHOORD is a string in the format\n"
       "                        <latitude>:<longitude> where the choordinates are given as floating\n"
       "                        point numbers\n"
       "  -m|--mode      MODE   One of print, print-hourly, plot-hourly, plot-daily, plot-precip-daily,\n"
       "                        plot-precip-hourly. Defaults to 'print'\n"
       "  -r|--request          By pass the cache if a cache file exists\n"
       "  -v|--version          Print program version and exit"
       );
}

int main(int argc, char **argv) {

  Config c = CONFIG_NULL;
  Data d = DATA_NULL;

  double cli_location[2] = { 0.0, 0.0 };
  int cli_mode = -1;
  int opt;
  int use_cli_location = 0;
  bool dump_data = false;
  bool free_c_path = false;
  bool bypass_cache = false;

  while((opt = getopt_long(argc, argv, options, options_long, NULL)) != -1) {
    switch(opt) {
      case 'h':
        usage();
        return EXIT_SUCCESS;
      case 'l':
        if(parse_location((const char*)optarg, &cli_location[0], &cli_location[1]) == -1)
          printf("-l: malformed option argument\n");
        use_cli_location = 1;
        break;
      case 'c':
        c.path = optarg;
        break;
      case 'v':
        puts(PACKAGE_STRING);
        puts("Compiled on: " __DATE__ " " __TIME__);
        return EXIT_SUCCESS;
      case '?':
        usage();
        return EXIT_FAILURE;
      case 'm':
        if((cli_mode = match_mode_arg((const char*)optarg)) == -1) {
          printf("-m: invalid mode %s, selecting default\n", optarg);
          cli_mode = OP_PRINT_CURRENTLY;
        }
        break;
      case 'd':
        dump_data = true;
        break;
      case 'r':
        bypass_cache = true;
        break;
    }
  }

  if(c.path == NULL) {
    int len = snprintf(NULL, 0, "%s/%s", getenv("HOME"), RCNAME) + 1;
    c.path = malloc(len);
    snprintf((char*)c.path, len, "%s/%s", getenv("HOME"), RCNAME);
    free_c_path = true;
  }

  if(load_config(&c) != 0)
    puts("Failed to load configuration");

  if(strlen(c.apikey) == 0)
    LERROR(EXIT_FAILURE, 0, "API key must not be empty.");
  if(string_isalnum(c.apikey) == -1)
    LERROR(EXIT_FAILURE, 0, "API key is not a hexstring.", c.apikey);

  if(cli_mode != -1)
    c.op = cli_mode;

  if(use_cli_location == 1) {
    c.location.latitude = cli_location[0];
    c.location.longitude = cli_location[1];
  }

  if(bypass_cache == true || load_cache(&c, &d) == -1) {
    if(request(&c, &d) != 0)
      puts("Failed to request data");
    else
      save_cache(&c, &d);
  }

  if(dump_data) {
    write(STDOUT_FILENO, d.data, d.datalen);
    putchar('\n');
  } else
    render(&c, &d);

  if(d.data != NULL) {
    free(d.data);
  }

  FREE_IF(free_c_path, c.path);
  free_config(&c);

  return EXIT_SUCCESS;
}
