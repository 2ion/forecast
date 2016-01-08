#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string.h>

enum {
  LANG_AR           = 0x00,
  LANG_BS           = 0x01,
  LANG_DE           = 0x02,
  LANG_EN           = 0x03,
  LANG_ES           = 0x04,
  LANG_FR           = 0x05,
  LANG_IT           = 0x06,
  LANG_NL           = 0x07,
  LANG_PL           = 0x08,
  LANG_PT           = 0x09,
  LANG_RU           = 0x0a,
  LANG_SK           = 0x0b,
  LANG_SV           = 0x0c,
  LANG_TET          = 0x0d,
  LANG_TR           = 0x0e,
  LANG_UK           = 0x0f,
  LANG_X_PIG_LATIN  = 0x10,
  LANG_ZH           = 0x11,
  LANG_ZH_TW        = 0x12,
  LANG_HR           = 0x13
};

int match_lang_arg(const char*);
const char* lang_name(int);

#endif
