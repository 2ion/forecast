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
void    render_precipitation_plot_daily(const PlotCfg *, struct json_object*);
void    render_precipitation_plot_hourly(const PlotCfg *, struct json_object*);

#endif
