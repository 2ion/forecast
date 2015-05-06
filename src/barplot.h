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
#include <string.h>
#include <unistd.h>

enum {
  PLOT_COLOR_BAR            = 1,
  PLOT_COLOR_LEGEND         = 2,
  PLOT_COLOR_TEXTHIGHLIGHT  = 3,
  PLOT_COLOR_BAR_OVERLAY    = 4,
  PLOT_COLOR_PRECIP         = 5
};

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
    .label_format = NULL              \
  },                                  \
  .daily = {                          \
    .label_format = NULL              \
  },                                  \
  .precipitation = {                  \
    .label_format = NULL,             \
    .bar_color = COLOR_BLUE           \
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
    char *label_format;
  } hourly;
  struct {
    char *label_format;
  } daily;
  struct {
    char *label_format;
    int bar_color;
  } precipitation;
} PlotCfg;

void barplot(const PlotCfg *c, const double *d, size_t dlen);
void barplot2(const PlotCfg *c, const double *d, char **labels, size_t dlen, int color);
void barplot_overlaid(const PlotCfg *c, const double *d1, const double *d2, char **labels, size_t dlen);
void barplot_horizontal_partitions(const PlotCfg *c, double min, double max, const double **pairs, char **labels, size_t len);
int terminal_dimen(int *rows, int *cols);

#endif
