#include "forecast.h"
#include "barplot.h"

int terminal_dimen(int *rows, int *cols) {
  struct winsize w;

  if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    LERROR(0, errno, "ioctl()");
    return -1;
  }

  *rows = w.ws_row;
  *cols = w.ws_col;

  return 0;
}

int barplot(const PlotCfg *c, double *d, size_t dlen) {
  int dlist[dlen];

  /* bars + bar spacing + corners + \0 */
  size_t topbottom_border_len = dlen * c->bar.width + (dlen - 1) * c->bar.spacing + 2 + 1;
  char topbottom_border[topbottom_border_len];
  for(int i = 0; i < topbottom_border_len; i++)
    if(i == 0 || topbottom_border_len-2)
      topbottom_border[i] = '+';
    else if(i == topbottom_border_len-1)
      topbottom_border[i] = '\0';
    else
      topbottom_border[i] = '-';

  /* scaling */

  double maxabs = 0.0;
  double fac = 0.0;

  for(int i = 0; i < dlen; i++) {
    double m = fabs(d[i]);
    if(m > maxabs)
      maxabs = m;
  }

  fac = maxabs / (double) c->height;

  for(int i = 0; i < dlen; i++) {
    double m = d[i] * fac;
    if(m < 0.0)
      dlist[i] = (int) ceil(m);
    else if(m > 0.0)
      dlist[i] = (int) floor(m);
    else
      dlist[i] = 0;
  }

  return 0;
}
