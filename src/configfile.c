#include "configfile.h"

int load_config(Config *c) {
  assert(c);

  config_t cfg;
  const char *apikey;
  const char *tmp;

  if(access(c->path, R_OK) != 0) {
    LERROR(0, errno, "access()");
    return -1;
  }

  config_init(&cfg);

  if(config_read_file(&cfg, c->path) != CONFIG_TRUE) {
    LERROR(0, 0, "[%s, %d] %s",
        config_error_file(&cfg),
        config_error_line(&cfg),
        config_error_text(&cfg));
    goto return_error;
  }

  if(config_lookup_string(&cfg, "apikey", &apikey) != CONFIG_TRUE) {
    LERROR(0, 0, "No API key found.");
    goto return_error;
  }

  if((c->apikey = malloc(strlen(apikey) + 1)) == NULL)
    LERROR(EXIT_FAILURE, errno, "malloc()");

  memcpy((void*)c->apikey, apikey, strlen(apikey) + 1);

  if(config_lookup_float(&cfg, "location.latitude", &(c->location.latitude)) != CONFIG_TRUE) {
    LERROR(0, 0, "location.latitude not configured");
    goto return_error;
  }

  if(config_lookup_float(&cfg, "location.longitude", &(c->location.longitude)) != CONFIG_TRUE) {
    LERROR(0, 0, "location.longitude not configured");
    goto return_error;
  }

  if(config_lookup_int(&cfg, "plot.height", &(c->plot.height)) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.height not configured");
    goto return_error;
  }

  if(config_lookup_string(&cfg, "plot.bar.color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.bar.color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.bar.color)
  }

  if(config_lookup_string(&cfg, "plot.bar.overlay_color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.bar.overlay_color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.bar.overlay_color)
  }

  if(config_lookup_string(&cfg, "plot.legend.texthighlight_color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.legend.texthighlight_color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.legend.texthighlight_color)
  }

  if(config_lookup_int(&cfg, "plot.bar.width", &(c->plot.bar.width)) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.bar.width");
    goto return_error;
  }

  if(config_lookup_string(&cfg, "plot.legend.color", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.legend.color");
    goto return_error;
  } else {
    CHECKCOLORS(c->plot.legend.color)
  }

  if(config_lookup_string(&cfg, "op", &tmp) != CONFIG_TRUE) {
    LERROR(0, 0, "op");
    goto return_error;
  } else {
    c->op = match_mode_arg(tmp);
    if(c->op == -1)
      goto return_error;
  }

  if(config_lookup_int(&cfg, "plot.hourly.succeeding_hours", &(c->plot.hourly.succeeding_hours)) != CONFIG_TRUE) {
    LERROR(0, 0, "plot.hourly.succeeding_hours");
    goto return_error;
  }

  config_destroy(&cfg);
  return 0;

return_error:
  config_destroy(&cfg);
  return -1;
}

int match_mode_arg(const char *str) {
  if(strcmp(str, "plot-hourly") == 0)
    return OP_PLOT_HOURLY;
  else if(strcmp(str, "print") == 0)
    return OP_PRINT_CURRENTLY;
  else if(strcmp(str, "print-hourly") == 0)
    return OP_PRINT_HOURLY;
  else
    return -1;
}

