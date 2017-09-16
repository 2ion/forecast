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

static const PlotCfg *PC;
static WINDOW *PAD;

void barplot_legend(int dx, int dy, int height, double dmax, double dmin) {
  const int rfac = dmin < 0.0 ? 2 : 1;

  wattron(PAD, COLOR_PAIR(PLOT_COLOR_LEGEND));
  for(int y = dy; y <= dy + rfac*height; y++) {
    if(y == dy + height) { /* zero-baseline */
      wattron(PAD, COLOR_PAIR(PLOT_COLOR_TEXTHIGHLIGHT));
      mvwaddch(PAD, y, dx-2, '+');
      wattroff(PAD, COLOR_PAIR(PLOT_COLOR_TEXTHIGHLIGHT));
      wattron(PAD, COLOR_PAIR(PLOT_COLOR_LEGEND));
      mvwprintw(PAD, y, dx-6, "0.0");
    } else if(y == dy) { /* y-axis maximum */
      mvwaddch(PAD, y, dx-2, '|');
      mvwprintw(PAD, y,
          dx-(snprintf(NULL, 0, "%.*f", 1, dmax)+3),
          "%.*f", 1, dmax);
    } else if(y == dy + 2*height) { /* y-axis minimum */
      mvwaddch(PAD, y, dx-2, '|');
      mvwprintw(PAD, y,
          dx-(snprintf(NULL, 0, "-%.*f", 1, dmax)+3),
          "-%.*f", 1, dmax);
    } else
      mvwaddch(PAD, y, dx-2, '|');
  }
  wattroff(PAD, COLOR_PAIR(PLOT_COLOR_LEGEND));
}

void barplot_start(const PlotCfg *pc) {
  int default_color;
  WINDOW *w;

  PC = pc;

  /* if initscr() call fails, the program will terminate */
  setlocale(LC_ALL, "");
  w = initscr();
  PAD = newpad(LINES, COLS);

  cbreak();
  mousemask(ALL_MOUSE_EVENTS, NULL);
  mouseinterval(0);
  noecho();
  nonl();
  curs_set(0);

  scrollok(PAD, TRUE);
  intrflush(PAD, FALSE);
  keypad(PAD, TRUE);

  /* use colors and, if possible, terminal default colors */
  start_color();
  default_color = use_default_colors() == OK ? -1 : 0;
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
  wclear(PAD);
}

void barplot_pause(void)
{
  MEVENT ev;
  int c;
  bool use_mouse = has_mouse() == TRUE;

  while(c = wgetch(PAD)) {
    switch(c) {
      case KEY_MOUSE:
        if(use_mouse && getmouse(&ev) == OK) {
          int s = ERR;
          if(ev.bstate & BUTTON4_PRESSED)
            s = scrl(1);
          else if(ev.bstate & BUTTON5_PRESSED)
            s = scrl(-1);
        }
        break;
      case ' ':
      case 'q':
        goto end;
    }
  }

end:;

  return;
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
