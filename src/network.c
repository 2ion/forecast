#include "network.h"

size_t request_curl_callback(void *ptr, size_t size, size_t nmemb, void *data) {
  Data *d = (Data*) data;
  size_t ptrlen = size * nmemb;

  if(d->data == NULL) {
    d->data = malloc(ptrlen);
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
