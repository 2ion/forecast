#include "data.h"

void copy_data(const Data* d1, Data *d2) {
  d2->data = malloc(d1->datalen);
  GUARD_MALLOC(d2->data);
  memcpy(d2->data, (const void*) d1->data, d1->datalen);
  d2->datalen = d1->datalen;
}

void compress_data(Data *d) {
  int slen = d->datalen;
  int olen = LZ4_COMPRESSBOUND(slen);
  char cbuf[olen];

#if LZ4_VERSION_MINOR >= 7
  int ocnt = LZ4_compress_default((const char*)d->data, cbuf, slen, olen);
#else
  /* In this API version, $cbuf is required to be large enough to handle
   * up to LZ4_COMPRESSBOUND($slen) bytes. Since we already allocate
   * this large of a buffer anyway, we just drop $olen.
   *
   * This is required for building against the liblz4 version in Debian
   * Jessie.
   */
  int ocnt = LZ4_compress((const char*)d->data, cbuf, slen);
#endif

  if(ocnt == 0) /* should never happen */
    LERROR(EXIT_FAILURE, 0, "LZ4_compress_default() failed");

  free(d->data);
  d->data = malloc(ocnt);
  GUARD_MALLOC(d->data);
  d->datalen = (size_t)ocnt;
  memcpy(d->data, (const void*)cbuf, d->datalen);
}

void decompress_data(Data *d) {
  int obuflen = 0;
  int ocnt = 0;
  int lz4tries = 3; /* abort */
  char *obuf = NULL;

  do {
    obuflen += 0xFA00;
    if(obuf)
      free(obuf);
    obuf = malloc(obuflen);
    GUARD_MALLOC(obuf);
  } while((ocnt = LZ4_decompress_safe((const char*)d->data,
        obuf, d->datalen, obuflen)) < 0 && (lz4tries-- > 0));

  free(d->data);
  d->datalen = ocnt;
  d->data = malloc(d->datalen);
  GUARD_MALLOC(d->data);
  memcpy(d->data, (const void*) obuf, d->datalen);
  free(obuf);
}

