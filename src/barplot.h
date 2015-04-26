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
  PLOT_COLOR_BAR_OVERLAY    = 4
};

#define PLOTCFG_DEFAULT         \
{                               \
  .height = 6,                  \
  .bar = {                      \
    .width = 2,                 \
    .color = COLOR_BLACK,       \
    .overlay_color = COLOR_RED  \
  },                            \
  .legend = {                   \
    .color = COLOR_WHITE        \
  },                            \
  .hourly = {                   \
    .succeeding_hours = 10      \
  }                             \
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
  } legend;
  struct {
    int succeeding_hours;
  } hourly;
} PlotCfg;

void barplot(const PlotCfg *c, const double *d, size_t dlen);
void barplot_overlaid(const PlotCfg *c, const double *d1, const double *d2, size_t dlen);
int terminal_dimen(int *rows, int *cols);

#endif
