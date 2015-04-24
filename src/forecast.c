#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "barplot.h"
#include "configfile.h"
#include "forecast.h"
#include "network.h"
#include "render.h"

/* globals */

static const char *options = "hl:c:k:vCH:D:p:";
static const struct option options_long[] = {
  { "help",     no_argument,        NULL, 'h' },
  { "location", required_argument,  NULL, 'l' },
  { "config",   required_argument,  NULL, 'c' },
  { "api-key",  required_argument,  NULL, 'k' },
  { "version",  no_argument,        NULL, 'v' },
  { "current",  no_argument,        NULL, 'C' },
  { "hourly",   required_argument,  NULL, 'H' },
  { "daily",    required_argument,  NULL, 'D' },
  { "plot",     required_argument,  NULL, 'p' },
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
       "  -C|--current          Show the current weather condition\n"
       "  -H|--hourly    N      Show the weather forecast for the next N hours\n"
       "  -D|--daily     N      Show the weather forecast for the next N days\n"
       "  -c|--config    PATH   Configuration file to use\n"
       "  -h|--help             Print this message and exit\n"
       "  -l|--location  CHOORD Query the weather at this location; CHOORD is a string in the format\n"
       "                        <latitude>:<longitude> where the choordinates are given as floating\n"
       "                        point numbers\n"
       "  -k|--key       APIKEY API key to use\n"
       "  -v|--version          Print program version and exit"
       );
}

int main(int argc, char **argv) {
  Config c = CONFIG_NULL;
  Data d = DATA_NULL;

  char *cli_apikey = NULL;
  double cli_location[2] = { 0.0, 0.0 };
  int opt;
  int use_cli_location = 0;

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
        return EXIT_SUCCESS;
      case '?':
        usage();
        return EXIT_FAILURE;
      case 'm':
        if(strcmp(optarg, "plot-hourly") == 0)
          c.op = OP_PLOT_HOURLY;
        else if(strcmp(optarg, "print") == 0)
          c.op = OP_PRINT_CURRENTLY;
        else if(strcmp(optarg, "print-hourly") == 0)
          c.op = OP_PRINT_HOURLY;
        break;
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

  if(use_cli_location == 1) {
    c.location.latitude = cli_location[0];
    c.location.longitude = cli_location[1];
  }

  if(request(&c, &d) != 0)
    puts("Failed to request data");

  render(&c, &d);

  if(d.data != NULL)
    free(d.data);

  PlotCfg pc = PLOTCFG_DEFAULT;
  pc.height = 20;
  pc.bar.width = 4;
  double t[7] = { 7.0, 13.0, 19.7, 20.0, 21.0, 10.0, -30.0};
  barplot(&pc, t, 7);

  return EXIT_SUCCESS;
}
