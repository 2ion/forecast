#include "tree.h"

TLocation* tree_new(const Data *d)
{
  return NULL;
}

int* tree_int(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return &(t[i]->value.i);
  return NULL;
}

char* tree_char(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return t[i]->value.s;
  return NULL;
}

double* tree_float(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return &(t[i]->value.d);
  return NULL;
}


