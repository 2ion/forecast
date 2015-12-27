#include "fs.h"

int main(int argc, char**argv)
{
  Config c = CONFIG_NULL;
  Data d = DATA_NULL;
  TLocation *root;

  set_config_path(&c);
  if(load_config(&c) != 0)
    LERROR(EXIT_FAILURE, 0, "Failed to load the configuration file");

  request(&c, &d);

  root = tree_new(&d);

  tree_print(root, stdout);

  tree_free(root);
  free(d.data);
  free_config(&c);

  return 0;
}
