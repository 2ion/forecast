#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <libconfig.h>
#include <string.h>
#include <assert.h>

#include "forecast.h"

int load_config(Config *c);
void free_config(Config *c);
int match_mode_arg(const char *str);

#endif
