#include "tree.h"

static TALLOC_CTX*  access_string(struct json_object*, const char*, TALLOC_CTX*);
static int          access_int(struct json_object*, const char*);
static double       access_double(struct json_object*, const char*);
static size_t       json_object_length(struct json_object *o);
static TData**      parse_hourly_object(struct json_object*, TALLOC_CTX*, size_t*);
static TData**      parse_daily_object(struct json_object*, TALLOC_CTX*, size_t*);
static void         print_tdata_array(TData**, size_t, size_t, FILE*);
static int          compare_against_array(const char**,  const char*);

static const struct { char *tee; char *branch; } graphc = { "├", "──" };
static const char *string_keys[] = { "summary", "icon", "precipType", NULL };

/*********************************************************************/

TLocation* tree_new(const char *location_name, const Data *d)
{
  TLocation *l;
  struct json_object *o, *oo, *ooo;
  struct array_list *al;

  if((o = json_tokener_parse(d->data)) == NULL)
    return NULL;

  if((l = talloc(NULL, TLocation)) == NULL)
    return NULL;

  l->name = talloc_strdup(l, location_name);
  l->json_data = talloc_strndup(l, d->data, d->datalen);
  l->json_data_len = d->datalen;

  /* top-level data */

  l->timezone = access_string(o, "timezone", l);
  l->latitude = access_double(o, "latitude");
  l->longitude = access_double(o, "longitude");
  l->offset = access_int(o, "offset");

  /* .currently */

  json_object_object_get_ex(o, "currently", &oo);
  l->w_currently = parse_hourly_object(oo, l, &l->w_currently_len);

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

char* tree_char(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return t[i]->value.s;
  return NULL;
}

double* tree_double(TData **t, size_t tlen, const char *key)
{
  for(size_t i = 0; i < tlen; i++)
    if(strcmp(t[i]->name, key) == 0)
      return &(t[i]->value.d);
  return NULL;
}

void tree_print(TLocation *root, FILE *stream)
{
  fprintf(stream,
      "units      %s\n"
      "latitude   %.2f\n"
      "longitude  %.2f\n"
      "timezone   %s\n"
      "tz offset  %d\n",
      root->units,
      root->latitude,
      root->longitude,
      root->timezone,
      root->offset);

  fputs("currently\n", stream);
  print_tdata_array(root->w_currently, root->w_currently_len, 2, stream);

  fputs("hourly\n", stream);
  for(size_t i = 0; i < root->w_hourly_len; i++)
  {
    fputs("---\n", stream);
    print_tdata_array(root->w_hourly[i], root->w_hourly_chld_len[i], 2, stream);
  }

  fputs("daily\n", stream);
  for(size_t i = 0; i < root->w_daily_len; i++)
  {
    fputs("---\n", stream);
    print_tdata_array(root->w_daily[i], root->w_daily_chld_len[i], 2, stream);
  }
}

/*********************************************************************/
/*                          static functions                         */

int compare_against_array(const char **array, const char *needle)
{
  for(size_t i = 0; array[i]; i++) {
    if(strcmp(array[i], needle) == 0)
      return 0;
  }
  return 1;
}

void print_tdata_array(TData **a, size_t alen, size_t indent, FILE *stream)
{
  char sep[indent+1];

  memset(sep, ' ', indent);
  sep[indent] = '\0';

  for(size_t i = 0; i < alen; i++)
  {
    switch(a[i]->type)
    {
      case TD_DOUBLE:
        fprintf(stream, "%s%s%s %s: %.2f\n",
            sep,
            graphc.tee,
            graphc.branch,
            a[i]->name,
            a[i]->value.d);
        break;
      case TD_STRING:
        fprintf(stream, "%s%s%s %s: %s\n",
            sep,
            graphc.tee,
            graphc.branch,
            a[i]->name,
            a[i]->value.s);
        break;
    }
  }
}


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
    if(compare_against_array(string_keys, td->name) == 0)
    {
      td->value.s = talloc_strdup(td, json_object_get_string(v));
      td->type = TD_STRING;
    }
    else
    {
      td->value.d = json_object_get_double(v);
      td->type = TD_DOUBLE;
    }
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
    if(compare_against_array(string_keys, td->name) == 0)
    {
      td->value.s = talloc_strdup(td, json_object_get_string(v));
      td->type = TD_STRING;
    }
    else
    {
      td->value.d = json_object_get_double(v);
      td->type = TD_DOUBLE;
    }
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

