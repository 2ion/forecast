#ifndef RENDER_H
#define RENDER_H

#include <assert.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <string.h>
#include <time.h>

#include "forecast.h"

char*   render_time(struct json_object*);
double  render_f2c(double fahrenheit);
double  render_mph2kph(double mph);
int     render(const Config *c, Data *d);
int     render_datapoint(struct json_object *d);
void    render_hourly_datapoints(struct json_object*);
void    render_hourly_datapoints_plot(const PlotCfg*, struct json_object*);
void    render_daily_temperature_plot(const PlotCfg*, struct json_object*);

#endif
