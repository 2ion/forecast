#ifndef CACHE_H
#define CACHE_H

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lz4.h>

#include "forecast.h"
#include "hash.h"
#include "network.h"
#include "util.h"
#include "tree.h"

TLocation* cache_fill(const Config*);

#endif
