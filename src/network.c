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

#define QUERY_VIRGIN "?%s=%s"
#define QUERY_NEXT "&%s=%s"
#define QUERYSTR(f) (f?(QUERY_VIRGIN):(QUERY_NEXT))

static size_t request_curl_callback(void*, size_t, size_t, void*);
static char* add_query_param(const char*, const char*, const char*);
static char* subststr(char*, char*);

char* subststr(char *o, char *n) {
  free(o);
  return n;
}

char* add_query_param(const char *s, const char *k, const char *v) {
  const bool f = strchr((const char*)s, '?') == NULL ? true : false;
  size_t elen;
  char *ebuf, *obuf;

  elen = snprintf(NULL, 0, QUERYSTR(f), k, v);
  ebuf = malloc(elen + 1);
  GUARD_MALLOC(ebuf);
  snprintf(ebuf, elen + 1, QUERYSTR(f), k, v);

  obuf = malloc(strlen(s) + elen + 1);
  GUARD_MALLOC(obuf);
  memcpy(obuf, s, strlen(s));
  memcpy(&obuf[strlen(s)], ebuf, elen + 1);

  free(ebuf);
  return obuf;
}

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
  const char *url_template = "https://api.forecast.io/forecast/%s/%f,%f";
  int urllen;
  char *url;
  CURLcode r;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  urllen = snprintf(NULL, 0, url_template, c->apikey, c->location.latitude, c->location.longitude) + 1;
  url = malloc(urllen);
  GUARD_MALLOC(url);
  snprintf(url, urllen, url_template, c->apikey, c->location.latitude, c->location.longitude);

  /* QUERY PARAMETERS */

  if(c->language != LANG_EN)
    url = subststr(url, add_query_param((const char*)url, "lang", lang_name(c->language)));

  if(c->extend_hourly)
    url = subststr(url, add_query_param((const char*)url, "extend", "hourly"));

  if(c->units != UNITS_AUTO)
    url = subststr(url, add_query_param((const char*)url, "units", UNIT_STR(c->units)));

  /*******************/

  CURL *curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, request_curl_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, d);

  if((r = curl_easy_perform(curl)) != CURLE_OK)
    printf("cURL error: %s\n", curl_easy_strerror(r));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  free(url);

  return r == CURLE_OK ? 0 : -1;
};
