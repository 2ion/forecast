#ifndef BARPLOT_H
#define BARPLOT_H

#include <stdbool.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>

#define PLOTCFG_DEFAULT       \
{                             \
  .height = 6,                \
  .baseline = BASELINE_ZERO,  \
  .bar = {                    \
    .width = 1,               \
    .spacing = 1,             \
    .show_value = false       \
  }                           \
}

enum {
  BASELINE_MEAN,
  BASELINE_ZERO
};

typedef struct {
  int height;
  int baseline;
  struct {
    int width;
    int spacing;
    bool show_value;
  } bar;
} PlotCfg;

int barplot(const PlotCfg *c, double *d, size_t dlen);
int terminal_dimen(int *rows, int *cols);

#endif
