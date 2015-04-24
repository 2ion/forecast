#ifndef BARPLOT_H
#define BARPLOT_H

#include <stdbool.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>

#define PLOTCFG_DEFAULT       \
{                             \
  .height = 6,                \
  .bar = {                    \
    .width = 2,               \
    .color = COLOR_GREEN      \
  },                          \
  .legend = {                 \
    .color = COLOR_WHITE      \
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
} PlotCfg;

int barplot(const PlotCfg *c, double *d, size_t dlen);
int terminal_dimen(int *rows, int *cols);

#endif
