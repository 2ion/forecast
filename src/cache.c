#include "cache.h"

static int check_cache_file(const Config*);

int check_cache_file(const Config *c) {
  struct stat s;
  struct timeval tv;

  /* Check cache file accssibility */
  if(access(c->cache_file, F_OK | R_OK) != 0)
    return -1;

  /* Check cache file age */
  if(stat(c->cache_file, &s) != 0)
    return -1;
  gettimeofday(&tv, NULL);
  if((tv.tv_sec - s.st_mtim.tv_sec) >= c->max_cache_age)
    return -1;

  return 0;
}

int load_cache(const Config *c, Data *d) {
  FILE *cf;
  long cflen;

  if(check_cache_file(c) != 0)
    return  -1;

  if((cf = fopen(c->cache_file, "rb")) == NULL) {
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

  return 0;
}

int save_cache(const Config *c, const Data *d) {
  int fd;
  int ret = 0;

  if((fd = open(c->cache_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1)
    return -1;

  ret = write(fd, (const void*) d->data, d->datalen);

  if(ret == -1 || ret < d->datalen) {
    LERROR(0, errno, "write()");
    ret = -1;
  }

  close(fd);

  return ret;
}
