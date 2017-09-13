#include "fs.h"

/*
 * Forecast File System (FFS)
 *
 *  +
 *  | + locations
 *  | | + ${location-name}
 *  | | | name
 *  | | | timezone
 *  | | | timezone_offset
 *  | | | latitude
 *  | | | longitude
 *  | | | cache_file
 *  | | | json_data
 *  | | + weather/
 *  | |   | current/
 *  | |   | hourly/
 *  | |   | daily/
 *  | |
 *  | + parameters # read/write for changing
 *  | | api_key
 *  | | timezone
 *  | | configuration_path
 *  | | update_interval
 *  | | units
 *  | /exec # touch $file for 
 *  | | flush_cache
 *  | /callback
 *  | | on_refresh $0 $mount-point $location-name
 *  | | on_exit $0
 */

/*
 * forecastfs
 *  --timezone=geo|system|custom
 *  --config=$path
 *  --update-interval=$seconds
 *  --api-key=$key
 *  --units=
 */

int main(int argc, char**argv)
{
  Config c = CONFIG_NULL;
  Data d = {0};
  TLocation *root;

  set_config_path(&c);
  if(load_config(&c) != 0)
    exit(1);


  request(&c, &d);
  root = tree_new("default_location", &d);
  tree_print(root, stdout);

  tree_free(root);
  free(d.data);
  free_config(&c);

  return 0;
}
