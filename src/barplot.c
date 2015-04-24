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

  /* scaling */

  double maxabs = 0.0;

  for(int i = 0; i < dlen; i++) {
    double m = fabs(d[i]);
    if(m > maxabs)
      maxabs = m;
  }

  const double fac = (double) c->height / maxabs;
  int maxdlist = 0;

  for(int i = 0; i < dlen; i++) {
    double m = d[i] * fac;
    if(m < 0.0)
      dlist[i] = (int) ceil(m);
    else if(m > 0.0)
      dlist[i] = (int) floor(m);
    else
      dlist[i] = 0;
    if(dlist[i] > maxdlist)
      maxdlist = dlist[i];
    LERROR(0,0, "d[%d]: %f => %d", i, d[i], dlist[i]);
  }


  /* tic labels on y axis */

  double ticnames[c->height + 1];
  ticnames[0] = 0.0;
  for(int i = 1; i <= c->height; i++) {
    ticnames[i] = (1.0/fac) * (double) i;
    LERROR(0,0, "ticnames[%d] = %f", i, ticnames[i]);
  }

  /* curses */

  setlocale(LC_ALL, "");
  WINDOW *w = initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, -1, COLOR_BLUE);
  init_pair(2, COLOR_WHITE, -1);
  init_pair(3, COLOR_RED, -1);

  int offset = 0;
  const int dx = COLS/2 - (dlen * (c->bar.width + 1) - 1)/2;
  const int dy = LINES/2 - c->height;

  /* plot the decoration and legend */

  attron(COLOR_PAIR(2));
  for(int y = dy; y <= dy + 2*c->height; y++) {

    if(y == dy + c->height) {

      attron(COLOR_PAIR(3));
      mvaddch(y, dx-2, '+');
      attroff(COLOR_PAIR(3));
      attron(COLOR_PAIR(2));

      mvprintw(y, dx-6, "0.0");

    } else if(y == dy) {

      mvaddch(y, dx-2, '-');
      mvprintw(y,
          dx-(snprintf(NULL, 0, "%.*f", 1, ticnames[c->height])+3),
          "%.*f", 1, ticnames[c->height]);

    } else if(y == dy + 2*c->height) {

      mvaddch(y, dx-2, '-');
      mvprintw(y,
          dx-(snprintf(NULL, 0, "-%.*f", 1, ticnames[c->height])+3),
          "-%.*f", 1, ticnames[c->height]);

    } else {

      mvaddch(y, dx-2, '|');

    }
  }
  attroff(COLOR_PAIR(2));

  for(int i = 0; i < dlen; i++) {
    const int d = dlist[i] >= 0 ? 1 : -1;
    const int _offset = offset;

    for(int j = dx + i + offset; j < dx + i + c->bar.width + _offset; j++, offset++) {
    /* plot the zero-line */
    attron(COLOR_PAIR(2));
    mvaddch(dy + c->height, j, '-');
    attroff(COLOR_PAIR(2));
    /* plot the bar */
    attron(COLOR_PAIR(1));
    for(int y = dy + c->height - dlist[i]; y != dy + c->height; y += d)
      mvaddch(y, j, ' ');
    attroff(COLOR_PAIR(1));
    }

  }

  refresh();
  getch();
  endwin();

  return 0;
}
