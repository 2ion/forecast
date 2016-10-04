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

#include "forecast.h"
#include "barplot.h"

static void barplot_scale(const double*, size_t, int, int*, double*, double*, double*);
static void barplot_legend(int dx, int dy, int height, double dmax, double dmin);
static double frac_of_day_mins(const struct tm *t);

static const PlotCfg *PC;

double frac_of_day_mins(const struct tm *t) {
  return (t->tm_hour*60 + t->tm_min) /(24.0*1440.0);
}

void barplot_legend(int dx, int dy, int height, double dmax, double dmin) {
  const int rfac = dmin < 0.0 ? 2 : 1;

  attron(COLOR_PAIR(PLOT_COLOR_LEGEND));
  for(int y = dy; y <= dy + rfac*height; y++) {
    if(y == dy + height) { /* zero-baseline */
      attron(COLOR_PAIR(PLOT_COLOR_TEXTHIGHLIGHT));
      mvaddch(y, dx-2, '+');
      attroff(COLOR_PAIR(PLOT_COLOR_TEXTHIGHLIGHT));
      attron(COLOR_PAIR(PLOT_COLOR_LEGEND));
      mvprintw(y, dx-6, "0.0");
    } else if(y == dy) { /* y-axis maximum */
      mvaddch(y, dx-2, '|');
      mvprintw(y,
          dx-(snprintf(NULL, 0, "%.*f", 1, dmax)+3),
          "%.*f", 1, dmax);
    } else if(y == dy + 2*height) { /* y-axis minimum */
      mvaddch(y, dx-2, '|');
      mvprintw(y,
          dx-(snprintf(NULL, 0, "-%.*f", 1, dmax)+3),
          "-%.*f", 1, dmax);
    } else
      mvaddch(y, dx-2, '|');
  }
  attroff(COLOR_PAIR(PLOT_COLOR_LEGEND));
}

void barplot_start(const PlotCfg *pc) {
  int default_color;

  PC = pc;

  /* if this call fails, the program will terminate */
  initscr();

  /* screen and echo setup */
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);

  /* hide cursor */
  curs_set(0);

  /* use colors and, if possible, terminal default colors */
  start_color();
  default_color = use_default_colors() == OK ? -1 : 0;

  /* colors defined in the config file */
  init_pair(PLOT_COLOR_BAR,           default_color,                  PC->bar.color);
  init_pair(PLOT_COLOR_LEGEND,        PC->legend.color,               default_color);
  init_pair(PLOT_COLOR_TEXTHIGHLIGHT, PC->legend.texthighlight_color, default_color);
  init_pair(PLOT_COLOR_BAR_OVERLAY,   default_color,                  PC->bar.overlay_color);
  init_pair(PLOT_COLOR_PRECIP,        default_color,                  PC->precipitation.bar_color);
  init_pair(PLOT_COLOR_DAYLIGHT,      PC->daylight.color,             default_color);
}

void barplot_end(void)
{
  endwin();
}

void barplot_clear(void)
{
  clear();
}

void barplot_pause(void)
{
  getch();
}

void barplot_msgbox(const char *msg, int valign, int halign)
{
  refresh();
}

void barplot_scale(const double *d, size_t dlen, int scaleheight,
    int *scaled, double *scalefac, double *max, double *min)
{
  for(int i = 0; i < dlen; i++) {
    double m = fabs(d[i]);
    if(m > *max)
      *max = m;
    else if(m < *min)
      *min = m;
  }

  *scalefac = (double) scaleheight / (*max);

  for(int i = 0; i < dlen; i++) {
    double m = d[i] * (*scalefac);
    if(m < 0.0)
      scaled[i] = (int) ceil(m);
    else if(m > 0.0)
      scaled[i] = (int) floor(m);
    else
      scaled[i] = 0;
    if(scaled[i] > (int) *max)
      *max = (double) scaled[i];
    else if(scaled[i] < 0)
      *min = (double) scaled[i];
  }
}

void barplot_simple(const double *d, char **labels, size_t dlen,
    int bar_color)
{
  int ds[dlen];
  double sfac, dmax, dmin;

  barplot_scale(d, dlen, PC->height, &ds[0], &sfac, &dmax, &dmin);

  const int dx = COLS/2 - (dlen * (PC->bar.width + 1) - 1)/2;
  const int dy = LINES/2 - PC->height;

  barplot_legend(dx, dy, PC->height, dmax, dmin);

  int offset = 0;
  for(int i = 0; i < dlen; i++) {
    const int delta = ds[i] >= 0 ? 1 : -1;
    const int _offset = offset;

    attron(COLOR_PAIR(PLOT_COLOR_LEGEND));
    mvprintw(dy + PC->height, dx + i + offset, "%s", labels[i]);
    attroff(COLOR_PAIR(PLOT_COLOR_LEGEND));

    for(int j = dx + i + offset; j < dx + i + PC->bar.width + _offset; j++, offset++) {
      attron(COLOR_PAIR(bar_color));
      for(int y = dy + PC->height - ds[i]; y != dy + PC->height; y += delta)
        mvaddch(y, j, ' ');
      attroff(COLOR_PAIR(bar_color));
    }
  }

  refresh();
}

void barplot_overlaid(const double *d1, const double *d2, char **labels,
    size_t dlen)
{
  double  d[2*dlen];
  int     ds[2*dlen];
  double  sfac;
  double  dmax;
  double  dmin;

  memcpy(&d, d1, dlen * sizeof(double));
  memcpy(&d[dlen], d2, dlen * sizeof(double));

  barplot_scale(d, 2*dlen, PC->height, &ds[0], &sfac, &dmax, &dmin);

  const int dx = COLS/2 - (dlen * (PC->bar.width + 1) - 1)/2;
  const int dy = LINES/2 - PC->height;

  barplot_legend(dx, dy, PC->height, dmax, dmin);

  int offset = 0;
  for(int i = 0; i < dlen; i++) {
    const int _offset = offset;

    attron(COLOR_PAIR(PLOT_COLOR_LEGEND));
    mvprintw(dy + PC->height, dx + i + offset, "%s", labels[i]);
    attroff(COLOR_PAIR(PLOT_COLOR_LEGEND));

    for(int j = dx + i + offset; j < dx + i + PC->bar.width + _offset; j++, offset++) {
      for(int k = 0; k < 2; k++) {
        const int idx = i + k * dlen;
        const int d = ds[idx] >= 0 ? 1 : -1;
        const int barcoloridx = (k == 0) ? PLOT_COLOR_BAR : PLOT_COLOR_BAR_OVERLAY;

        attron(COLOR_PAIR(barcoloridx));
        for(int y = dy + PC->height - ds[idx]; y != dy + PC->height; y += d)
          mvaddch(y, j, ' ');
        attroff(COLOR_PAIR(barcoloridx));
      } // for k
    } // for j
  } // for i

  refresh();
}
