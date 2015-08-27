#include "cache.h"
#include "hash.h"

static int check_cache_file(const Config *c, const char*);
static char* get_cache_file_path(const Config*);
static void compress_data(Data *d);
static void decompress_data(Data *d);
static void copy_data(const Data*, Data*);

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
#define FORECAST_LZ4_COMPRESS LZ4_compress_default
#else
#define FORECAST_LZ4_COMPRESS LZ4_compress
#endif
  int ocnt = FORECAST_LZ4_COMPRESS((const char*)d->data,
      cbuf, slen, olen);
#undef FORECAST_LZ4_COMPRESS

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

char* get_cache_file_path(const Config *c) {
  char lohash[33];
  char *buf;
  int buflen;

  /* hash the location's string representation */
  buflen = snprintf(NULL, 0, "%f:%f", c->location.latitude,
      c->location.longitude);
  buf = malloc(buflen + 1);
  snprintf(buf, buflen + 1, "%f:%f", c->location.latitude,
      c->location.longitude);
  if(md5str((const char*)buf, lohash, sizeof(lohash)) != 0)
    return NULL;
  free(buf);

  /* find the cachefile for this exact location */
  buflen = snprintf(NULL, 0, c->cache_file, lohash);
  buf = malloc(buflen + 1);
  GUARD_MALLOC(buf);
  snprintf(buf, buflen + 1, c->cache_file, lohash);

  /* free after use */
  return buf;
}

int check_cache_file(const Config *c, const char* path) {
  struct stat s;
  struct timeval tv;

  /* Check cache file accssibility */
  if(access(path, F_OK | R_OK) != 0)
    return -1;

  /* Check cache file age */
  if(stat(path, &s) != 0)
    return -1;
  gettimeofday(&tv, NULL);
  if((tv.tv_sec - s.st_mtim.tv_sec) >= c->max_cache_age)
    return -1;

  return 0;
}

int load_cache(const Config *c, Data *d) {
  FILE *cf;
  long cflen;

  const char *cache_file = get_cache_file_path(c);
  if(!cache_file) return -1;

  if(check_cache_file(c, cache_file) != 0)
    return  -1;

  if((cf = fopen(cache_file, "rb")) == NULL) {
    LERROR(0, errno, "fopen()");
    return -1;
  }

  fseek(cf, 0, SEEK_END);
  cflen = ftell(cf);
  fseek(cf, 0, SEEK_SET);

  d->data = malloc(cflen + 1);
  GUARD_MALLOC(d->data);
  d->datalen = cflen;
  fread(d->data, cflen, 1, cf);
  fclose(cf);

  decompress_data(d);

  free((void*)cache_file);
  return 0;
}

int save_cache(const Config *c, const Data *d) {
  int fd;
  int ret = 0;
  Data _d;

  const char *cache_file = get_cache_file_path(c);
  if(!cache_file) return -1;


  if((fd = open(cache_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
    return -1;

  copy_data(d, &_d);
  compress_data(&_d);
  ret = write(fd, (const void*) _d.data, _d.datalen);

  if(ret == -1 || ret < _d.datalen) {
    LERROR(0, errno, "write()");
    ret = -1;
  }

  close(fd);

  free(_d.data);
  free((void*)cache_file);
  return ret;
}
