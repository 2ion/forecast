#ifndef FS_H
#define FS_H

#include "config.h" /* FUSE_USE_VERSION */
#include <fuse.h>
#include <talloc.h>
#include <syslog.h>

#include "configfile.h"
#include "network.h"
#include "tree.h"

struct ffs_state
{
  TALLOC_CTX *ctx;
};

#endif
