#ifndef FORECAST_H
#define FORECAST_H

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
  struct json_object *(NAME(object, key)) = NULL;                   \
  json_object_object_get_ex((object), #key, &(NAME(object, key)));

#define RENDER_BEARING(deg) \
  deg ==  0.00 ? "N" : (deg <  45.0 ? "NNE" : (deg ==  45.0 ? "NE" : (deg <  90.0 ? "ENE" : \
( deg ==  90.0 ? "E" : (deg < 135.0 ? "ESE" : (deg == 135.0 ? "SE" : (deg < 180.0 ? "SSE" : \
( deg == 180.0 ? "S" : (deg < 225.0 ? "SSW" : (deg == 225.0 ? "SW" : (deg < 270.0 ? "WSW" : \
( deg == 270.0 ? "W" : (deg < 315.0 ? "WNW" : (deg == 315.0 ? "NW" : "NNW"))))))))))))))

#define COLOR_BLOCK "\x1b[0;0;42m "
#define COLOR_RESET "\x1b[0m"

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

#endif


