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

#include "hash.h"

/* buf must be of length 33 */
int md5str(const char *str, char *buf, size_t buflen) {
  if(!str||!buf||buflen<33) return -1;

  unsigned char s[strlen(str)];
  unsigned char d[16];

  for(size_t i = 0; i < strlen(str); i++)
    s[i] = (unsigned char) str[i];

  MD5(s, (unsigned long)strlen(str), d);

  for(int b = 0; b < 16; b++)
    snprintf(&buf[2*b], 3, "%02x", (unsigned)d[b]);

  return 0;
}
