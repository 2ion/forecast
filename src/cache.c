#include "cache.h"
#include "hash.h"

static int check_cache_file(const Config *c, const char*);
static char* get_cache_file_path(const Config*);

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

  free((void*)cache_file);
  return 0;
}

int save_cache(const Config *c, const Data *d) {
  int fd;
  int ret = 0;

  const char *cache_file = get_cache_file_path(c);
  if(!cache_file) return -1;

  if((fd = open(cache_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
    return -1;

  ret = write(fd, (const void*) d->data, d->datalen);

  if(ret == -1 || ret < d->datalen) {
    LERROR(0, errno, "write()");
    ret = -1;
  }

  close(fd);

  free((void*)cache_file);
  return ret;
}
