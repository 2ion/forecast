#ifndef NETWORK_H
#define NETWORK_H

#include <curl/curl.h>
#include <string.h>

#include "forecast.h"

int    request(Config *c, Data *d);
size_t request_curl_callback(void*, size_t, size_t, void*);

#endif
