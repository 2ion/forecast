#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include "forecast.h"

/* Looks for needle in an unsorted, NULL-terminated array of strings */
int compare_against_array(const char **array, const char *needle);
void copy_data(const Data*, Data*);
/* temporary sibling until everything has been migrated to libtalloc */
void xcopy_data(TALLOC_CTX *parent, const Data *src, Data *dst);

#endif
