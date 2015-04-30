#ifndef CACHE_H
#define CACHE_H

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "forecast.h"

int load_cache(const Config*, Data*);
int save_cache(const Config*, const Data*);

#endif
