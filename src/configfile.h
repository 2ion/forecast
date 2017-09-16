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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <assert.h>
#include <ctype.h>
#include <libconfig.h>
#include <string.h>
#include <talloc.h>

#include "forecast.h"
#include "units.h"

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

int load_config(TALLOC_CTX *rx, Config *c);
void free_config(Config *c);
int match_mode_arg(const char *str);
int string_isalnum(const char *str);

#endif
