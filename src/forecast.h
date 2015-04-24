#ifndef FORECAST_H
#define FORECAST_H

#include <errno.h>
#include <error.h>
#include <sys/types.h>
#include <stdbool.h>

#include "config.h"
#include "barplot.h"

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

typedef struct {
  const char *path;
  const char *apikey;
  struct {
    double latitude;
    double longitude;
  } location;
  PlotCfg plot;
} Config;

typedef struct {
  char *data;
  size_t datalen;
} Data;

#endif


