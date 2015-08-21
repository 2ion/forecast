#ifndef HASH_H
#define HASH_H

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <openssl/md5.h>

int md5str(const char *str, char *buf, size_t buflen);

#endif
