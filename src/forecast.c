#include <assert.h>
#include <curl/curl.h>
#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <libconfig.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#define LERROR(status, errnum, ...) error_at_line((status), (errnum), \
        (__func__), (__LINE__), __VA_ARGS__)

#define _PASTE(x, y) x ## _ ## y

#define PASTE(x, y) _PASTE(x, y)

#define NAME(prefix, name) PASTE(prefix, name)

#define EXTRACT(object, key)  \
  struct json_object *(key);  \
  json_object_object_get_ex((object), #key, &(key));

#define EXTRACT_PREFIXED(object, key)                               \
  struct json_object *(NAME(object, key));                          \
  json_object_object_get_ex((object), #key, &(NAME(object, key)));

#define RENDER_BEARING(deg) \
  deg ==  0.00 ? "N" : (deg <  45.0 ? "NNE" : (deg ==  45.0 ? "NE" : (deg <  90.0 ? "ENE" : \
( deg ==  90.0 ? "E" : (deg < 135.0 ? "ESE" : (deg == 135.0 ? "SE" : (deg < 180.0 ? "SSE" : \
( deg == 180.0 ? "S" : (deg < 225.0 ? "SSW" : (deg == 225.0 ? "SW" : (deg < 270.0 ? "WSW" : \
( deg == 270.0 ? "W" : (deg < 315.0 ? "WNW" : (deg == 315.0 ? "NW" : "NNW"))))))))))))))

/* types */

typedef struct {
  const char *path;
  const char *apikey;
  struct {
    double latitude;
    double longitude;
  } location;
} Config;

typedef struct {
  char *data;
  size_t datalen;
} Data;

/* globals */

static const char *options = "hl:c:k:vCH:D:";
static const struct option options_long[] = {
  { "help",     no_argument,        NULL, 'h' },
  { "location", required_argument,  NULL, 'l' },
  { "config",   required_argument,  NULL, 'c' },
  { "api-key",  required_argument,  NULL, 'k' },
  { "version",  no_argument,        NULL, 'v' },
  { "current",  no_argument,        NULL, 'C' },
  { "hourly",   required_argument,  NULL, 'H' },
  { "daily",    required_argument,  NULL, 'D' }, 
  { 0,          0,                  0,    0   }
};

/* prototypes */

static char*  render_time(struct json_object*);
static double render_f2c(double fahrenheit);
static double render_mph2kph(double mph);
static int    load_config(Config *c);
static int    parse_location(const char *s, double *la, double *lo);
static int    render(Data *d);
static int    render_datapoint(struct json_object *d);
static int    request(Config *c, Data *d);
static size_t request_curl_callback(void*, size_t, size_t, void*);
static void   usage(void);

/* implementations */

double render_mph2kph(double mph) {
  return mph * 1.609344;
}

double render_f2c(double fahrenheit) {
  return (fahrenheit - 32.0) * 5.0/9.0;
}

char * render_time(struct json_object *timeptr) {
  time_t t = json_object_get_int(timeptr);
  return ctime(&t);
}

int render_datapoint(struct json_object *o) {
  assert(o);

  EXTRACT_PREFIXED(o, time);
  EXTRACT_PREFIXED(o, temperature);
  EXTRACT_PREFIXED(o, apparentTemperature);
  EXTRACT_PREFIXED(o, summary);
  EXTRACT_PREFIXED(o, dewPoint);
  EXTRACT_PREFIXED(o, humidity);
  EXTRACT_PREFIXED(o, precipProbability);
  EXTRACT_PREFIXED(o, cloudCover);
  EXTRACT_PREFIXED(o, windSpeed);
  EXTRACT_PREFIXED(o, pressure);
  EXTRACT_PREFIXED(o, ozone);
  EXTRACT_PREFIXED(o, windBearing); // FIXME: might be undefined

  printf( "   Time                  | %s"
          "   Condition             | %s\n"
          "   Temperature           | %.*f °C\n"
          "   Apparent temperature  | %.*f °C\n"
          "   Dew point             | %.*f °C\n"
          "   Precipitation         | %d %%\n"
          "   RH (φ)                | %.*f %%\n"
          "   Wind speed            | %d kph (%s)\n"
          "   Cloud cover           | %d %%\n"
          "   Pressure              | %.*f hPa\n"
          "   Ozone                 | %.*f DU\n",
              render_time(o_time),
              json_object_get_string(o_summary),
          1,  render_f2c(json_object_get_double(o_temperature)),
          1,  render_f2c(json_object_get_double(o_apparentTemperature)),
          1,  render_f2c(json_object_get_double(o_dewPoint)),
              (int) (json_object_get_double(o_precipProbability) * 100.0),
          1,  json_object_get_double(o_humidity) * 100,
              (int) render_mph2kph(json_object_get_double(o_windSpeed)),
              RENDER_BEARING(json_object_get_double(o_windBearing)),
              (int) (json_object_get_double(o_cloudCover) * 100.0),
          2,  json_object_get_double(o_pressure),
          2,  json_object_get_double(o_ozone)
        );
}

int render(Data *d) {
  struct json_object *o = json_tokener_parse(d->data);

  EXTRACT_PREFIXED(o, timezone);
  EXTRACT_PREFIXED(o, latitude);
  EXTRACT_PREFIXED(o, longitude);
  EXTRACT_PREFIXED(o, currently);

  printf( "Latitude                 | %.*f\n"
          "Longitude                | %.*f\n"
          "Timezone                 | %s\n"
          "Currently\n",
          4,  json_object_get_double(o_latitude),
          4,  json_object_get_double(o_longitude),
              json_object_get_string(o_timezone));

  render_datapoint(o_currently);

  return 0;
}

size_t request_curl_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
  Data *d = (Data*) data;
  size_t ptrlen = size * nmemb;

  if(d->data == NULL) {
    d->data = malloc(ptrlen);
    d->datalen = ptrlen;
    memcpy(d->data, ptr, ptrlen);
  } else {
    d->data = realloc(d->data, d->datalen + ptrlen);
    memcpy(&d->data[d->datalen], ptr, ptrlen);
    d->datalen += ptrlen;
  }

  return ptrlen;
}

int request(Config *c, Data *d) {
  int urllen;
  char *url;
  CURLcode r;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  urllen = snprintf(NULL, 0, "https://api.forecast.io/forecast/%s/%f,%f",
      c->apikey, c->location.latitude, c->location.longitude) + 1;
  url = malloc(urllen);
  snprintf(url, urllen, "https://api.forecast.io/forecast/%s/%f,%f",
      c->apikey, c->location.latitude, c->location.longitude);

  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, request_curl_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, d);
  r = curl_easy_perform(curl);
  if(r != CURLE_OK)
    printf("cURL error: %s\n", curl_easy_strerror(r));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  free(url);

  return r == CURLE_OK ? 0 : -1;
};

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

int load_config(Config *c) {
  assert(c);

  config_t cfg;
  const char *apikey;

  if(access(c->path, R_OK) != 0) {
    LERROR(0, errno, "access()");
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

  if((c->apikey = malloc(strlen(apikey) + 1)) == NULL)
    LERROR(EXIT_FAILURE, errno, "malloc()");

  memcpy((void*)c->apikey, apikey, strlen(apikey) + 1);

  if(config_lookup_float(&cfg, "location.latitude", &(c->location.latitude)) != CONFIG_TRUE) {
    LERROR(0, 0, "location.latitude not configured");
    goto return_error;
  }

  if(config_lookup_float(&cfg, "location.longitude", &(c->location.longitude)) != CONFIG_TRUE) {
    LERROR(0, 0, "location.longitude not configured");
    goto return_error;
  }

return_success:
  config_destroy(&cfg);
  return 0;

return_error:
  config_destroy(&cfg);
  return -1;
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
  Config c = { NULL, NULL, { 0.0, 0.0 } };
  Data d = { NULL, 0 };
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

  render(&d);

  return EXIT_SUCCESS;
}
