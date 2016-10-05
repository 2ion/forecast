#include "util.h"

int compare_against_array(const char **array, const char *needle)
{
  for(size_t i = 0; array[i]; i++) {
    if(strcmp(array[i], needle) == 0)
      return 0;
  }
  return 1;
}

void copy_data(const Data* d1, Data *d2)
{
  d2->data = malloc(d1->datalen);
  GUARD_MALLOC(d2->data);
  memcpy(d2->data, (const void*) d1->data, d1->datalen);
  d2->datalen = d1->datalen;
}

void xcopy_data(TALLOC_CTX *parent, const Data *src, Data *dst)
{
  dst->data = talloc_size(parent, src->datalen);
  GUARD_MALLOC(dst->data);
  memcpy(dst, (const void*) src->data, src->datalen);
  dst->datalen = src->datalen;
}
