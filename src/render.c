#include "render.h"

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

void render_hourly_datapoints(struct json_object *hourly) {
  assert(hourly);

  EXTRACT_PREFIXED(hourly, summary);
  EXTRACT_PREFIXED(hourly, data);
  struct array_list *al = json_object_get_array(hourly_data);

  puts(   "-------------------------+");
  printf( "Hourly                     %s\n", json_object_get_string(hourly_summary));

  for(int i = 0; i < array_list_length(al); i++) {
    struct json_object *o = array_list_get_idx(al, i);
    render_datapoint(o);
  }
}

void render_hourly_datapoints_plot(const PlotCfg *c, struct json_object *hourly) {
  assert(hourly);

  double buf[48];
  int i;
  double *data = &buf[0];
  size_t datalen = 48;
  int free_data = false;

  EXTRACT_PREFIXED(hourly, data);

  struct array_list *al = json_object_get_array(hourly_data);

  for(i = 0; i < array_list_length(al) && i < c->hourly.succeeding_hours + 1; i++) {
    struct json_object *o = array_list_get_idx(al, i);
    EXTRACT_PREFIXED(o, temperature);
    double v = json_object_get_double(o_temperature);
    if(i == datalen) {
      if(free_data == false) {
        data = malloc(2*datalen);
        memcpy(data, buf, datalen);
        datalen *= 2;
        free_data = true;
      } else {
        datalen *= 2;
        data = realloc(data, datalen);
      }
    }
    data[i] = render_f2c(v);
  } // for
  barplot(c, data, i);
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

  puts(   "-------------------------+");
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

int render(const Config *c, Data *d) {
  struct json_object *o = json_tokener_parse(d->data);

  EXTRACT_PREFIXED(o, timezone);
  EXTRACT_PREFIXED(o, latitude);
  EXTRACT_PREFIXED(o, longitude);
  EXTRACT_PREFIXED(o, currently);
  EXTRACT_PREFIXED(o, hourly);

#define PRINT_HEADER                                \
  printf( "Latitude                 | %.*f\n"       \
          "Longitude                | %.*f\n"       \
          "Timezone                 | %s\n"         \
          "-------------------------+\n"            \
          "Currently\n",                            \
          4,  json_object_get_double(o_latitude),   \
          4,  json_object_get_double(o_longitude),  \
              json_object_get_string(o_timezone));
  switch(c->op) {
    case OP_PRINT_CURRENTLY:
      PRINT_HEADER;
      render_datapoint(o_currently);
      break;
    case OP_PRINT_HOURLY:
      PRINT_HEADER;
      render_hourly_datapoints(o_hourly);
      break;
    case OP_PLOT_HOURLY:
      render_hourly_datapoints_plot(&c->plot, o_hourly);
      break;
  }
#undef PRINT_HEADER

  return 0;
}


