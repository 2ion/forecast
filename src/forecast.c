#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barplot.h"
#include "cachefile.h"
#include "configfile.h"
#include "forecast.h"
#include "network.h"
#include "render.h"

/* globals */

static const char *options = "hl:c:k:vm:d";
static const struct option options_long[] = {
  { "help",     no_argument,        NULL, 'h' },
  { "location", required_argument,  NULL, 'l' },
  { "config",   required_argument,  NULL, 'c' },
  { "api-key",  required_argument,  NULL, 'k' },
  { "version",  no_argument,        NULL, 'v' },
  { "mode",     required_argument,  NULL, 'm' },
  { "dump",     no_argument,        NULL, 'd' },
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
       "  forecast [-CHDchlkv] [OPTIONS]\n"
       "Options:\n"
       "  -c|--config    PATH   Configuration file to use\n"
       "  -d|--dump             Dump the JSON data and a newline to stdout\n"
       "  -h|--help             Print this message and exit\n"
       "  -l|--location  CHOORD Query the weather at this location; CHOORD is a string in the format\n"
       "                        <latitude>:<longitude> where the choordinates are given as floating\n"
       "                        point numbers\n"
       "  -m|--mode      MODE   One of print, print-hourly, plot-hourly. Defaults to 'print'\n"
       "  -k|--key       APIKEY API key to use\n"
       "  -v|--version          Print program version and exit"
       );
}

int main(int argc, char **argv) {

  Config c = CONFIG_NULL;
  Data d = DATA_NULL;

  char *cli_apikey = NULL;
  double cli_location[2] = { 0.0, 0.0 };
  int cli_mode = -1;
  int opt;
  int use_cli_location = 0;
  bool dump_data = false;

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
      case 'k':
        cli_apikey = optarg;
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
    }
  }

  if(c.path == NULL) {
    int len = snprintf(NULL, 0, "%s/%s", getenv("HOME"), RCNAME) + 1;
    c.path = malloc(len);
    snprintf((char*)c.path, len, "%s/%s", getenv("HOME"), RCNAME);
  }

  if(load_config(&c) != 0)
    puts("Failed to load configuration");

  if(cli_apikey) {
    free((void*)c.apikey);
    c.apikey = cli_apikey;
  }

  if(cli_mode != -1)
    c.op = cli_mode;

  if(use_cli_location == 1) {
    c.location.latitude = cli_location[0];
    c.location.longitude = cli_location[1];
  }

  if(request(&c, &d) != 0)
    puts("Failed to request data");

  if(dump_data) {
    write(STDOUT_FILENO, d.data, d.datalen);
    putchar('\n');
  } else
    render(&c, &d);

  if(d.data != NULL)
    free(d.data);

  if(c.plot.daily.label_format != NULL)
    free(c.plot.daily.label_format);

  return EXIT_SUCCESS;
}
