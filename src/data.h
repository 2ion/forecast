#pragma once

#include <lz4.h>
#include <stdlib.h>

#include "util.h"
#include "lerror.h"

typedef struct {
  char *data;
  size_t datalen;
} Data;

void copy_data(const Data*, Data*);
void compress_data(Data*);
void decompress_data(Data*);
