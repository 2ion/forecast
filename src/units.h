#ifndef UNITS_H
#define UNITS_H

#include <stdio.h>
#include <string.h>

/* global table of unit names */
const char **unit_table;
/******************************/

enum {
  UNITS_US,
  UNITS_SI,
  UNITS_CA,
  UNITS_UK2,
  UNITS_AUTO
};

#define UNIT_STR(e) \
  e == UNITS_US    ? "us"    : \
( e == UNITS_SI    ? "si"    : \
( e == UNITS_CA    ? "ca"    : \
( e == UNITS_UK2   ? "uk2"   : \
( e == UNITS_AUTO  ? "auto"  : \
                     NULL   ))))

enum {
  nearestStormDistance  = 0x0,
  precipIntensity       = 0x1,
  precipIntensityMax    = 0x2,
  precipAccumulation    = 0x3,
  temperature           = 0x4,
  temperatureMin        = 0x5,
  temperatureMax        = 0x6,
  apparentTemperature   = 0x7,
  dewPoint              = 0x8,
  windSpeed             = 0x9,
  pressure              = 0xA,
  visibility            = 0xB
};

int set_global_unit_table(int);
void show_unit_table(void);
int match_units_arg(const char *str);

#endif
