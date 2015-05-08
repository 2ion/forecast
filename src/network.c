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

#include "network.h"

size_t request_curl_callback(void *ptr, size_t size, size_t nmemb, void *data) {
  Data *d = (Data*) data;
  size_t ptrlen = size * nmemb;

  if(d->data == NULL) {
    d->data = malloc(ptrlen);
    GUARD_MALLOC(d->data);
    d->datalen = ptrlen;
    memcpy(d->data, ptr, ptrlen);
  } else {
    d->data = realloc(d->data, d->datalen + ptrlen);
    memcpy(&d->data[d->datalen], ptr, ptrlen);
    d->datalen += ptrlen;
  }

  return ptrlen;
}

int request(Config *c, Data *d) {
  int urllen;
  char *url;
  CURLcode r;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  urllen = snprintf(NULL, 0, "https://api.forecast.io/forecast/%s/%f,%f",
      c->apikey, c->location.latitude, c->location.longitude) + 1;
  url = malloc(urllen);
  GUARD_MALLOC(url);
  snprintf(url, urllen, "https://api.forecast.io/forecast/%s/%f,%f",
      c->apikey, c->location.latitude, c->location.longitude);

  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, request_curl_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, d);
  r = curl_easy_perform(curl);
  if(r != CURLE_OK)
    printf("cURL error: %s\n", curl_easy_strerror(r));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  free(url);

  return r == CURLE_OK ? 0 : -1;
};
