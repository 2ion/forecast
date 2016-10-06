#include "units.h"

static int set_global_unit_table(int);

/* global table of unit names */
const char **unit_table = NULL;
/******************************/

const char *unit_table_si[]   = { "km", "mm/h", "mm/h", "cm", "°C", "°C", "°C", "°C", "°C", "m/s",  "hPa",  "km"  };
const char *unit_table_ca[]   = { "km", "mm/h", "mm/h", "cm", "°C", "°C", "°C", "°C", "°C", "km/h", "hPa",  "km"  };
const char *unit_table_uk2[]  = { "m",  "mm/h", "mm/h", "cm", "°C", "°C", "°C", "°C", "°C", "mph",  "hPa",  "m"   }; /*miles*/
const char *unit_table_us[]   = { "m",  "in/h", "in/h", "in", "°F", "°F", "°F", "°F", "°F", "mph",  "mbar", "m"   }; /*miles*/

int set_global_unit_table(int u) {
  switch(u) {
    case UNITS_US:
      unit_table = unit_table_us;
      break;
    case UNITS_SI:
      unit_table = unit_table_si;
      break;
    case UNITS_CA:
      unit_table = unit_table_ca;
      break;
    case UNITS_UK2:
      unit_table = unit_table_uk2;
      break;
    default: /* unit code without associated table, ATM 'auto' */
      LERROR(0, 0, "No unit table for unit code '%s', defaulting to SI",
          UNIT_STR(u));
      unit_table = unit_table_si;
      return -1;
  }
  return 0;
}

void show_unit_table(void) {
#define SHOW_INDEX(i) printf("[" #i "] = %s\n", unit_table[i])
  SHOW_INDEX(nearestStormDistance);
  SHOW_INDEX(precipIntensity     );
  SHOW_INDEX(precipIntensityMax  );
  SHOW_INDEX(precipAccumulation  );
  SHOW_INDEX(temperature         );
  SHOW_INDEX(temperatureMin      );
  SHOW_INDEX(temperatureMax      );
  SHOW_INDEX(apparentTemperature );
  SHOW_INDEX(dewPoint            );
  SHOW_INDEX(windSpeed           );
  SHOW_INDEX(pressure            );
  SHOW_INDEX(visibility          );
#undef SHOW_INDEX
}

int match_units_arg(const char *str) {
  if(strcmp(str, "auto") == 0)
    return UNITS_AUTO;
  else if(strcmp(str, "uk") == 0)
    return UNITS_UK2;
  else if (strcmp(str, "us") == 0)
    return UNITS_US;
  else if (strcmp(str, "ca") == 0)
    return UNITS_CA;
  else if (strcmp(str, "si") == 0)
    return UNITS_SI;
  else
    return -1;
}

void learn_location_units(const TLocation *l, Config *c)
{
  if(c->units == UNITS_AUTO)
    c->units = match_units_arg(l->units);
  set_global_unit_table(c->units);
}
