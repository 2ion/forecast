#include "language.h"

const char *language_table[] = {
  "ar", "bs", "de", "en", "es",   "fr", "it", "nl",           "pl",
  "pt", "ru", "sk", "sv", "tet",  "tr", "uk", "x_pig_latin",  "zh",
  NULL };

int match_lang_arg(const char *s) {
  int i = -1;
  while(language_table[++i]) {
    if(strcmp(language_table[i], s) == 0)
      return i;
  }
  return -1;
}

const char* lang_name(int i) {
  if(i > sizeof(language_table))
    return NULL;
  return language_table[i];
}
