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

#include "render.h"

static char*        render_time(struct json_object*);
static const char*  get_location_name(const Config*, size_t);
static void         render_daily_temperature_plot(const PlotCfg*, struct json_object*);
static void         render_datapoint(struct json_object *d);
static void         render_daylight(const PlotCfg*, struct json_object*);
static void         render_hourly_datapoints(const PlotCfg*, struct json_object*);
static void         render_hourly_datapoints_plot(const PlotCfg*, struct json_object*);
static void         render_precipitation_plot_daily(const PlotCfg *, struct json_object*);
static void         render_precipitation_plot_hourly(const PlotCfg *, struct json_object*);

const char* get_location_name(const Config *c, size_t i) {
  if(i < c->location_map_len)
    return NULL;
  return c->location_map[i].name;
}

char * render_time(struct json_object *timeptr) {
  time_t t = json_object_get_int(timeptr);
  return ctime(&t);
}

void render_hourly_datapoints(const PlotCfg *pc, struct json_object *hourly) {
  assert(hourly);

  EXTRACT_PREFIXED(hourly, summary);
  EXTRACT_PREFIXED(hourly, data);
  struct array_list *al = json_object_get_array(hourly_data);

  printf( "Hourly                       %s\n", json_object_get_string(hourly_summary));

  for(int i = 0; i * pc->hourly.step < array_list_length(al); i++) {
    struct json_object *o = array_list_get_idx(al, i * pc->hourly.step);
    render_datapoint(o);
  }
}

void render_hourly_datapoints_plot(const PlotCfg *pc, struct json_object *hourly) {
  assert(hourly);

  EXTRACT_PREFIXED(hourly, data);

  struct array_list *al = json_object_get_array(hourly_data);

  const int hlen = array_list_length(al);
  double data[hlen];
  char labels[hlen][pc->bar.width+1];
  char *plabels[hlen];
  int i;

  for(i = 0; i * pc->hourly.step < hlen && i < pc->hourly.succeeding_hours + 1; i++) {
    struct json_object *o = array_list_get_idx(al, i * pc->hourly.step);

    EXTRACT_PREFIXED(o, temperature);
    EXTRACT_PREFIXED(o, time);

    data[i] = json_object_get_double(o_temperature);

    time_t unixtime = json_object_get_int(o_time);
    struct tm *time = gmtime(&unixtime);
    strftime(labels[i], pc->bar.width+1, pc->hourly.label_format?:"%H", time);
    labels[i][pc->bar.width] = '\0';
    plabels[i] = &labels[i][0];
  } // for

  BARPLOT(pc, barplot_simple(data, plabels, i, PLOT_COLOR_BAR));
}

void render_precipitation_plot_hourly(const PlotCfg *pc, struct json_object *o) {
  EXTRACT_PREFIXED(o, data);

  struct array_list *al = json_object_get_array(o_data);

  const int hlen = array_list_length(al);
  double d[hlen];
  char labels[hlen][pc->bar.width+1];
  char *plabels[hlen];
  int i;

  for(i = 0; i < pc->hourly.succeeding_hours + 1 && i * pc->hourly.step < hlen ; i++) {
    struct json_object *oo = array_list_get_idx(al, i * pc->hourly.step);

    EXTRACT_PREFIXED(oo, precipProbability);
    EXTRACT_PREFIXED(oo, time);

    d[i] = json_object_get_double(oo_precipProbability) * 100;

    time_t unixtime = json_object_get_int(oo_time);
    struct tm *time = gmtime(&unixtime);
    strftime(labels[i], pc->bar.width+1, pc->hourly.label_format?:"%d", time);
    labels[i][pc->bar.width] = '\0';
    plabels[i] = &labels[i][0];
  }

  BARPLOT(pc, barplot_simple(d, plabels, i, PLOT_COLOR_PRECIP));

  return;
}

void render_precipitation_plot_daily(const PlotCfg *pc, struct json_object *o) {
  EXTRACT_PREFIXED(o, data);

  struct array_list *al = json_object_get_array(o_data);
  int allen = array_list_length(al);

  double d[allen];
  char labels[allen][pc->bar.width+1];
  char *plabels[allen];

  for(int i = 0; i < allen; i++) {
    struct json_object *oo = array_list_get_idx(al, i);

    EXTRACT_PREFIXED(oo, precipProbability);
    EXTRACT_PREFIXED(oo, time);

    d[i] = json_object_get_double(oo_precipProbability) * 100;

    time_t unixtime = json_object_get_int(oo_time) + 86400;
    struct tm *time = gmtime(&unixtime);
    strftime(labels[i], pc->bar.width+1, pc->daily.label_format?:"%d", time);
    labels[i][pc->bar.width] = '\0';
    plabels[i] = &labels[i][0];
  }

  BARPLOT(pc, barplot_simple(d, plabels, allen, PLOT_COLOR_PRECIP));

  return;
}

void render_daily_temperature_plot(const PlotCfg *pc, struct json_object *daily) {
  double tempMin[7];
  double tempMax[7];
  char labels[7][pc->bar.width+1];
  char *plbl[7];

  EXTRACT_PREFIXED(daily, data);
  struct array_list *al = json_object_get_array(daily_data);

  for(int i = 0; i < 7; i++) {
    struct json_object *o = array_list_get_idx(al, i);

    EXTRACT_PREFIXED(o, temperatureMin)
    EXTRACT_PREFIXED(o, temperatureMax)
    EXTRACT_PREFIXED(o, time);

    tempMin[i] = json_object_get_double(o_temperatureMin);
    tempMax[i] = json_object_get_double(o_temperatureMax);

    time_t unixtime = json_object_get_int(o_time) + 86400;
    struct tm *time = gmtime(&unixtime);

    strftime(labels[i], pc->bar.width+1, pc->daily.label_format?:"%d", time);
    labels[i][pc->bar.width] = '\0';
    plbl[i] = &labels[i][0];
  }

  BARPLOT(pc, barplot_overlaid(tempMax, tempMin, plbl, 7));
}

void render_daylight(const PlotCfg *pc, struct json_object *daily) {
  EXTRACT_PREFIXED(daily, data);
  struct array_list *al = json_object_get_array(daily_data);
  int allen = array_list_length(al);
  int times[3*allen];
  int j = 0;

  for(int i = 0; i < allen; i++) {
    struct json_object *o  = array_list_get_idx(al, i);
    EXTRACT_PREFIXED(o, time);
    EXTRACT_PREFIXED(o, sunriseTime);
    EXTRACT_PREFIXED(o, sunsetTime);
    times[j++] = json_object_get_int(o_time);
    times[j++] = json_object_get_int(o_sunriseTime);
    times[j++] = json_object_get_int(o_sunsetTime);
  }

  //BARPLOT(pc, barplot_daylight((const int*) &times[0], allen));
}

void render_datapoint(struct json_object *o) {
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

  printf( "   Time                      %s"
          "     Condition               %s\n"
          "     Temperature             %.*f %s\n"
          "     Apparent temperature    %.*f %s\n"
          "     Dew point               %.*f %s\n"
          "     Precipitation           %d %%\n"
          "     RH (φ)                  %.*f %%\n"
          "     Wind speed              %d %s (%s)\n"
          "     Cloud cover             %d %%\n"
          "     Pressure                %.*f %s\n"
          "     Ozone                   %.*f DU\n",
              render_time(o_time),
              json_object_get_string(o_summary),
          1,  json_object_get_double(o_temperature), unit_table[temperature],
          1,  json_object_get_double(o_apparentTemperature), unit_table[apparentTemperature],
          1,  json_object_get_double(o_dewPoint), unit_table[dewPoint],
              (int) (json_object_get_double(o_precipProbability) * 100.0),
          1,  json_object_get_double(o_humidity) * 100,
              (int) json_object_get_double(o_windSpeed), unit_table[windSpeed], RENDER_BEARING(json_object_get_double(o_windBearing)),
              (int) (json_object_get_double(o_cloudCover) * 100.0),
          2,  json_object_get_double(o_pressure), unit_table[pressure],
          2,  json_object_get_double(o_ozone)
        );
}

int render(Config *c, Data *d) {
  struct json_object *o = json_tokener_parse(d->data);

  EXTRACT_PREFIXED(o, timezone);
  EXTRACT_PREFIXED(o, latitude);
  EXTRACT_PREFIXED(o, longitude);
  EXTRACT_PREFIXED(o, currently);
  EXTRACT_PREFIXED(o, hourly);
  EXTRACT_PREFIXED(o, daily);

#define PRINT_HEADER                                  \
  printf( "Latitude                     %.*f\n"       \
          "Longitude                    %.*f\n"       \
          "Timezone                     %s\n",        \
          4,  json_object_get_double(o_latitude),     \
          4,  json_object_get_double(o_longitude),    \
              json_object_get_string(o_timezone));
  switch(c->op) {
    case OP_PRINT_CURRENTLY:
      PRINT_HEADER;
      render_datapoint(o_currently);
      break;
    case OP_PRINT_HOURLY:
      PRINT_HEADER;
      render_hourly_datapoints(&c->plot, o_hourly);
      break;
    case OP_PLOT_HOURLY:
      render_hourly_datapoints_plot(&c->plot, o_hourly);
      break;
    case OP_PLOT_DAILY:
      render_daily_temperature_plot(&c->plot, o_daily);
      break;
    case OP_PLOT_PRECIPITATION_DAILY:
      render_precipitation_plot_daily(&c->plot, o_daily);
      break;
    case OP_PLOT_PRECIPITATION_HOURLY:
      render_precipitation_plot_hourly(&c->plot, o_hourly);
      break;
    case OP_PLOT_DAYLIGHT:
      render_daylight(&c->plot, o_daily);
      break;
  }
#undef PRINT_HEADER
  json_object_put(o); /* free(o) by decreasing the refcount */
  return 0;
}
