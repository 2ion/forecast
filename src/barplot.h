#ifndef BARPLOT_H
#define BARPLOT_H

#include <curses.h>
#include <locale.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#define PLOTCFG_DEFAULT       \
{                             \
  .height = 6,                \
  .bar = {                    \
    .width = 2,               \
    .color = COLOR_GREEN      \
  },                          \
  .legend = {                 \
    .color = COLOR_WHITE      \
  },                          \
  .hourly = {                 \
    .succeeding_hours = 10    \
  }                           \
}

typedef struct {
  int height;
  struct {
    int width;
    int color;
  } bar;
  struct {
    int color;
  } legend;
  struct {
    int succeeding_hours;
  } hourly;
} PlotCfg;

int barplot(const PlotCfg *c, double *d, size_t dlen);
int terminal_dimen(int *rows, int *cols);

#endif
