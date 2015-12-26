#include "tree.h"

static TALLOC_CTX*  access_string(struct json_object*, const char*, TALLOC_CTX*);
static int          access_int(struct json_object*, const char*);
static double       access_double(struct json_object*, const char*);
static size_t       json_object_length(struct json_object *o);
static TData**      parse_hourly_object(struct json_object*, TALLOC_CTX*, size_t*);
static TData**      parse_daily_object(struct json_object*, TALLOC_CTX*, size_t*);

/*********************************************************************/

TLocation* tree_new(const Data *d)
{
  TLocation *l;
  struct json_object *o, *oo, *ooo;
  struct array_list *al;

  if((o = json_tokener_parse(d->data)) == NULL)
    return NULL;

  if((l = talloc(NULL, TLocation)) == NULL)
    return NULL;

  /* top-level data */

  l->timezone = access_string(o, "timezone", l);
  l->latitude = access_double(o, "latitude");
  l->longitude = access_double(o, "longitude");
  l->offset = access_int(o, "offset");

  /* .currently */

  json_object_object_get_ex(o, "currently", &oo);
  l->w_currently = parse_hourly_object(o, l, &l->w_currently_len);

  /* .hourly */

  json_object_object_get_ex(o, "hourly", &oo);
  json_object_object_get_ex(oo, "summary", &ooo);
  l->w_hourly_summary = talloc_strdup(l, json_object_get_string(ooo));
  json_object_object_get_ex(oo, "data", &ooo);
  al = json_object_get_array(ooo);
  l->w_hourly_len = array_list_length(al);
  l->w_hourly_chld_len = talloc_array(l, size_t, l->w_hourly_len);
  l->w_hourly = talloc_array(l, TData**, l->w_hourly_len);
  for(size_t i = 0; i < l->w_hourly_len; i++)
  {
    struct json_object *p = array_list_get_idx(al, i);
    l->w_hourly[i] = parse_hourly_object(p, l->w_hourly, &(l->w_hourly_chld_len[i]));
  }

  /* .daily */

  json_object_object_get_ex(o, "daily", &oo);
  json_object_object_get_ex(oo, "summary", &ooo);
  l->w_daily_summary = talloc_strdup(l, json_object_get_string(ooo));
  json_object_object_get_ex(oo, "data", &ooo);
  al = json_object_get_array(ooo);
  l->w_daily_len = array_list_length(al);
  l->w_daily_chld_len = talloc_array(l, size_t, l->w_daily_len);
  l->w_daily = talloc_array(l, TData**, l->w_daily_len);
  for(size_t i = 0; i < l->w_daily_len; i++)
  {
    struct json_object *p = array_list_get_idx(al, i);
    l->w_daily[i] = parse_daily_object(p, l->w_daily, &(l->w_daily_chld_len[i]));
  }

  /* .flags */

  json_object_object_get_ex(o, "flags", &oo);
  l->units = access_string(oo, "units", l);

  /* free json-c data structures by decreasing the refcount */
  json_object_put(o);

  return l;
}

void tree_free(TLocation *l)
{
  talloc_free(l);
}

int* tree_int(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return &(t[i]->value.i);
  return NULL;
}

char* tree_char(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return t[i]->value.s;
  return NULL;
}

double* tree_float(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return &(t[i]->value.d);
  return NULL;
}

/*********************************************************************/

TData** parse_daily_object(struct json_object *o, TALLOC_CTX *parent, size_t *alen)
{
  TData **array;
  size_t i = 0;

  *alen = json_object_length(o);
  array = talloc_array(parent, TData*, *alen);

  json_object_object_foreach(o, k, v)
  {
    TData *td = talloc(array, TData);
    td->name = talloc_strdup(td, k);
    if(strcmp(td->name, "summary") == 0
        || strcmp(td->name, "icon") == 0
        || strcmp(td->name, "precipType") == 0)
      td->value.s = talloc_strdup(td, json_object_get_string(v));
    else
      td->value.d = json_object_get_double(v);
    array[i++] = td;
  }

  return array;
}

TData** parse_hourly_object(struct json_object *o, TALLOC_CTX *parent, size_t *alen)
{
  TData **array;
  size_t i = 0;

  *alen = json_object_length(o);
  array = talloc_array(parent, TData*, *alen);

  json_object_object_foreach(o, k, v)
  {
    TData *td = talloc(array, TData);
    td->name = talloc_strdup(td, k);
    if(strcmp(td->name, "summary") == 0
        || strcmp(td->name, "icon") == 0)
      td->value.s = talloc_strdup(td, json_object_get_string(v));
    else
      td->value.d = json_object_get_double(v);
    array[i++] = td;
  }

  return array;
}

size_t json_object_length(struct json_object *o)
{
  size_t len = 0;

  json_object_object_foreach(o, k, v)
  {
    len++;
  }

  return len;
}

TALLOC_CTX* access_string(struct json_object *o, const char *key, TALLOC_CTX *ctx)
{
  struct json_object *oo;
  if(json_object_object_get_ex(o, key, &oo) != TRUE)
    return NULL;
  TALLOC_CTX *s = talloc_strdup(ctx, json_object_get_string(oo));
  return s;
}

int access_int(struct json_object *o, const char *key)
{
  struct json_object *oo;
  if(json_object_object_get_ex(o, key, &oo) != TRUE)
    return 0;
  return json_object_get_int(oo);
}

double access_double(struct json_object *o, const char *key)
{
  struct json_object *oo;
  if(json_object_object_get_ex(o, key, &oo) != TRUE)
    return 0;
  return json_object_get_double(oo);
}

