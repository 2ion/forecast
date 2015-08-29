#include "units.h"

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
    default: /* unknown unit code */
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
