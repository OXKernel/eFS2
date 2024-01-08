#include <stdio.h>
#include <string.h>

#include "paths.h"
#include "bool.h"
#include "block.h"
#include "inode.h"
#include "compat.h"

// From errno.c
int errno = 0;

time_t ktime(void)
{
  return time(0);
}

