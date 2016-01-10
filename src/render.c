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
static const char*  get_location_name(const Config*);
static void         render_daily_temperature_plot(const PlotCfg*, struct json_object*);
static void         render_datapoint(FILE*, const TData**, size_t);
static void         render_daylight(const PlotCfg*, struct json_object*);
static void         render_hourly_datapoints(const PlotCfg*, const TData***, size_t*, size_t);
static void         render_hourly_datapoints_plot(const PlotCfg*, struct json_object*);
static void         render_precipitation_plot_daily(const PlotCfg *, struct json_object*);
static void         render_precipitation_plot_hourly(const PlotCfg *, struct json_object*);
static FILE*        columnate(void);
static int          unitidx(const char*);

/* text output configuration */
static const char *ignored_datapoint_fields[] = { "icon", NULL };
static const struct { const char* key; int unitidx; } key_unit_map[] = {
#define x(key) { #key, key },
  x(nearestStormDistance)
  x(precipIntensity     )
  x(precipIntensityMax  )
  x(precipAccumulation  )
  x(temperature         )
  x(temperatureMin      )
  x(temperatureMax      )
  x(apparentTemperature )
  x(dewPoint            )
  x(windSpeed           )
  x(pressure            )
  x(visibility          )
#undef ENTRY
  { NULL, 0 }
};

FILE* columnate(void) {
  return popen("column -t -s@", "w");
}

const char* get_location_name(const Config *c) {
  if(c->location_map_idx < 0)
    return NULL;
  return c->location_map[c->location_map_idx].name;
}

char * render_time(struct json_object *timeptr) {
  time_t t = json_object_get_int(timeptr);
  return ctime(&t);
}

void render_hourly_datapoints(const PlotCfg *pc, const TData ***tdp, size_t *tdlen, size_t tdplen) {
  /*
  EXTRACT_PREFIXED(hourly, summary);
  EXTRACT_PREFIXED(hourly, data);
  struct array_list *al = json_object_get_array(hourly_data);

  printf( "Hourly                       %s\n", json_object_get_string(hourly_summary));

  for(int i = 0; i * pc->hourly.step < array_list_length(al); i++) {
    struct json_object *o = array_list_get_idx(al, i * pc->hourly.step);
    render_datapoint(o);
  }
  */
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

  barplot2(pc, data, plabels, i, pc->bar.color);
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

  barplot2(pc, d, plabels, i, PLOT_COLOR_PRECIP);

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

  barplot2(pc, d, plabels, allen, PLOT_COLOR_PRECIP);

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

  barplot_overlaid(pc, tempMax, tempMin, plbl, 7);
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

  barplot_daylight(pc, (const int*) &times[0], allen);
}

void render_datapoint(FILE *stream, const TData **td, size_t td_len) {
  return;
}

int render(Config *c, Data *d) {
  FILE *cf = NULL;
  const char *location_name = get_location_name(c);
  TLocation *tl = location_name ?
    tree_new(location_name, d) : tree_new("default", d);

  if(tl == NULL)
    return -1;

  /* learn unit table */
  if(c->units == UNITS_AUTO) {
    c->units = match_units_arg(tl->units);
  }
  set_global_unit_table(c->units);

  /* for textual output only */
  if(c->op == OP_PRINT_CURRENTLY || c->op == OP_PRINT_HOURLY)
    if((cf = columnate()) == NULL) {
      LERROR(0, errno,
          "popen() to column(1) failed: falling back to uncolumnated output");
      cf = stdout;
    }

#define PRINT_HEADER do{ fprintf(cf, "Latitude@%.*f\nLongitude@%.*f\nTimezone@%s\n", \
                       4, tl->latitude, 4, tl->longitude, tl->timezone); } while(0);
  switch(c->op) {
    case OP_PRINT_CURRENTLY:
      PRINT_HEADER;
      render_datapoint(cf, (const TData**)tl->w_currently, tl->w_currently_len);
      break;
    case OP_PRINT_HOURLY:
      PRINT_HEADER;
      render_hourly_datapoints(&c->plot, (const TData***)tl->w_hourly,
          tl->w_hourly_chld_len, tl->w_hourly_len);
      break;
      /*
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
      */
  }
#undef PRINT_HEADER

  if(cf != NULL)
    pclose(cf);

  tree_free(tl);
  return 0;
}
