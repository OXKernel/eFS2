//
// @file:
//      block.h
//
// @description:
//      Buffer cache storing disk read/writes in RAM for
//      optimization.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _BLOCK_H
#define _BLOCK_H

typedef unsigned long block_t;

// The number of blocks stored.
#define BLOCK_ARRAY_SIZE 4096
#define BLOCK_CACHE_SIZE 1031 // Prime. As we are hashing into a table.
#define BLOCK_FREE         -1
#define BLOCK_MAX_LRU    (block_t)4294967295 // 2147483647
#define BLOCK_NOPOS        -1

typedef enum block_rtvl {
  BLOCK_OK    = 0,
  BLOCK_FAIL  = -1,
  BLOCK_PARAM = -2,
  BLOCK_DEV   = -3,
  BLOCK_HASH  = -4,
  BLOCK_READ  = -5,
  BLOCK_LOGIC = -6
} block_rtvl_t;

block_rtvl_t block_open(char *path, int *dev);

block_rtvl_t block_close(int dev);

block_rtvl_t block_read(int dev, block_t block, char *data);

block_rtvl_t block_write(int dev, block_t block, char *data);

block_rtvl_t block_free(int dev, block_t block);

//
// block_sync:
// Go through entire cache, write every block to disk,
// reset dirty bit to be clean.
//
block_rtvl_t block_sync(int dev);

//
// block_disk_write:
//
// Force a disk write, normally a write means write to the cache.
//
block_rtvl_t block_disk_write(int dev, block_t block, char *data);

void block_reinit();

#endif
