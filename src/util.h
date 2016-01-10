#ifndef UTIL_H
#define UTIL_H

#include <string.h>

/* Looks for needle in an unsorted, NULL-terminated array of strings */
int compare_against_array(const char **array, const char *needle);

#endif
