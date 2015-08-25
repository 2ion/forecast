#include "hash.h"

/* buf must be of length 33 */
int md5str(const char *str, char *buf, size_t buflen) {
  if(!str||!buf||buflen<33) return -1;

  unsigned char s[strlen(str)];
  unsigned char d[16];

  for(int i = 0; i < strlen(str); i++)
    s[i] = (unsigned char) str[i];

  MD5(s, (unsigned long)strlen(str), d);

  for(int b = 0; b < 16; b++)
    snprintf(&buf[2*b], 3, "%02x", (unsigned)d[b]);

  return 0;
}
