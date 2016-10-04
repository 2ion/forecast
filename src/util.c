#include "util.h"

int compare_against_array(const char **array, const char *needle)
{
  for(size_t i = 0; array[i]; i++) {
    if(strcmp(array[i], needle) == 0)
      return 0;
  }
  return 1;
}
