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

#ifndef BARPLOT_H
#define BARPLOT_H

#include <sys/ioctl.h>
#include <sys/types.h>

#include <curses.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#define BARPLOT(pc, func)   \
  (func);                   \
  barplot_pause();

/* This are ncurses color pair indices */
enum {
  PLOT_COLOR_BAR            = 1,
  PLOT_COLOR_LEGEND         = 2,
  PLOT_COLOR_TEXTHIGHLIGHT  = 3,
  PLOT_COLOR_BAR_OVERLAY    = 4,
  PLOT_COLOR_PRECIP         = 5,
  PLOT_COLOR_DAYLIGHT       = 6
};

enum { TOP, MIDDLE, BOTTOM, LEFT, CENTER, RIGHT };

#define PLOTCFG_DEFAULT               \
{                                     \
  .height = 6,                        \
  .bar = {                            \
    .width = 2,                       \
    .color = COLOR_BLACK,             \
    .overlay_color = COLOR_RED        \
  },                                  \
  .legend = {                         \
    .color = COLOR_WHITE,             \
    .texthighlight_color = COLOR_RED  \
  },                                  \
  .hourly = {                         \
    .succeeding_hours = 10,           \
    .label_format = NULL,             \
    .step = 2                         \
  },                                  \
  .daily = {                          \
    .label_format = NULL              \
  },                                  \
  .precipitation = {                  \
    .label_format = NULL,             \
    .bar_color = COLOR_BLUE           \
  },                                  \
  .daylight = {                       \
    .width_frac = 0.75,               \
    .width_max = 72,                  \
    .date_label_format = NULL,        \
    .time_label_format = NULL,        \
    .color = COLOR_YELLOW             \
  }                                   \
}

typedef struct {
  int height;
  struct {
    int width;
    int color;
    int overlay_color;
  } bar;
  struct {
    int color;
    int texthighlight_color;
  } legend;
  struct {
    int succeeding_hours;
    int step;
    char *label_format;
  } hourly;
  struct {
    char *label_format;
  } daily;
  struct {
    char *label_format;
    int bar_color;
  } precipitation;
  struct {
    double width_frac;
    int width_max;
    char *date_label_format;
    char *time_label_format;
    int color;
  } daylight;
} PlotCfg;

void barplot_start(const PlotCfg*);
void barplot_end(void);
void barplot_msgbox(const char* msg, int valign, int halign);
void barplot_clear(void);
void barplot_pause(void);
void barplot_simple(const double *d, char **labels,
    size_t dlen, int color);
void barplot_overlaid(const double *d1,
    const double *d2, char **labels, size_t dlen);
void barplot_daylight(const int *times, size_t dlen);

#define barplot_print(fmt, ...) printw((fmt), __VA_ARGS__)

#endif
