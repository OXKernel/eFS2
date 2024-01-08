//
// @file:
//      block.c
//
// @description:
//      Buffer cache storing disk read/writes in RAM for
//      optimization. The cache supports multiple devices
//      as indicated by an 'int dev' parameter as well
//      as LRU swaps of blocks when the cache is full.
//      It does not use dynamic memory. It is intended
//      to interface directly low-level device read/writes.
//      Initially these devices will be floppy disk and
//      hard disk.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#include "block.h"
#include "lru.h"
#include "dev.h"
#include "bool.h"

#ifdef _TEST_BLOCK_INC
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define printk printf
#else
#include <ox/error_rpt.h>
#endif

// TODO - Figure out what ox kernel headers are needed.

static bool init = false;
static block_t  curr = 0;
static block_t highest = 0; // Highest LRU value.
static queue_t *block_dev [BLOCK_ARRAY_SIZE] = {0}; // Device the block is on.
static hash_t  *block_hash[BLOCK_ARRAY_SIZE] = {0}; // Device the block is on.

// RGDTODO change block_open (remove our lru logic, but open the device)
//         change block_sync (just call block_disk_write on all frames in Q)
//         change  or remove block_disk_write_sync_cache
//         block_close should be same once we implement block_sync
//         block_free I think this is being handled in our lru.c
//
//         Change lru.c programming style to use lru.h and use snake case
//         - The Queue and lru datastructures, are to be contained inside the lru.c
//         as block.c only exposes the block API.
//          **We have created lru.h and added the datastructures in it. This is fine for now.
//            we also didn't use snake_case, we will leave that until we test and make
//            sure all is ok.
//
//         - memset the data on QNode creation.
//           **Done, now uses calloc**
block_rtvl_t block_open(char *path, int *dev)
{
  register block_t i = 0;
  if(!path || !dev) {
    return BLOCK_PARAM;
  }
  if(!init) {
    // RGDTODO * We need to remove the use of dirty, block_lru and possibly block_map and see if we can use block_dev
    //         Here we should add the use of the lru.c code
    //         * create array of block_dev that is Queue object, we use max of BLOCK_ARRAY_SIZE elements,
    //         since we do not allocate all frames at once (it's done dynamically) we can have the same
    //         number of devices, and set them to DEV_NODEV.
    //         Queue *block_dev[BLOCK_ARRAY_SIZE]; // Init to DEV_NODEV
    //         * replace with logic from lru.c, it should be simpler, and make sure our block_* support
    //         methods in lru.c work and are called.
    for(i = 0; i < BLOCK_ARRAY_SIZE; i++) {
      // Initialize each block which is of size 'DEV_BLOCK_SIZE'
      // which in turn must be the size of the disk sector read/write
      // historically 512 bytes. New hard drives may now support 4096 bytes.
      block_hash[i] = NULL;
      block_dev[i]  = NULL;
    }
    init = true;
  }
  // TODO - Open the device. In user space, a file, in kernel
  // this must initialize the driver to read from the specified
  // device. The path should allow us to lookup a hard disk
  // and initialize the driver for it. We therefore need a table
  // to help us map say '/' with the main hard disk.
  if(dev_open(path, dev) == DEV_OK) {
    dev_scan(*dev, 0);
    if(!block_hash[*dev] && !block_dev[*dev]) {
      block_hash[*dev] = create_hash(BLOCK_CACHE_SIZE);
      block_dev[*dev]  = create_queue(*dev, BLOCK_CACHE_SIZE);
    } else {
      // This is a logic error, opening dev with existing cache open.
      dev_close(*dev);
      return BLOCK_FAIL;
    }
    return BLOCK_OK;
  } else {
    // TODO - Kernel should panic here or at least tell us
    //        that the device failed.
    return BLOCK_FAIL;
  }
}

block_rtvl_t block_read(int dev, block_t block, char *data)
{
  if(dev < 0 || block < 0 || !data) {
    return BLOCK_PARAM;
  }

  if(!block_dev[dev]) {
    return BLOCK_DEV;
  }
  if(!block_hash[dev]) {
    return BLOCK_HASH;
  }
  queue_t *queue = block_dev[dev];
  hash_t  *hash  = block_hash[dev];
  block_rtvl_t rtvl = BLOCK_OK;
  if((rtvl=reference_page(queue, hash, block)) != BLOCK_OK) {
    return rtvl; // Failed.
  }
  qnode_t * req_page = queue->front;
  if(!req_page || req_page->page_number != block) {
    // Error couldn't read from memory or disk.
    // This logic is in ReferencePage and Enqueue.
    return BLOCK_READ;
  } else {
    // Here we found the block in the cache.
    memcpy(data, req_page->data, DEV_BLOCK_SIZE);
    return BLOCK_OK;
  }
  // If we got here the cached read has failed.
  // This is logically not possible.
  return BLOCK_FAIL;
}

block_rtvl_t block_write(int dev, block_t block, char *data)
{
  if(dev < 0 || block < 0 || !data) {
    return BLOCK_PARAM;
  }

  if(!block_dev[dev]) {
    return BLOCK_DEV;
  }
  if(!block_hash[dev]) {
    return BLOCK_HASH;
  }
  queue_t *queue = block_dev[dev];
  hash_t  *hash  = block_hash[dev];
  block_rtvl_t rtvl = BLOCK_OK;
  if((rtvl=reference_page(queue, hash, block)) != BLOCK_OK) {
    return rtvl; // Failed.
  }
  qnode_t * req_page = queue->front;
  if(!req_page || req_page->page_number != block) {
    return BLOCK_FAIL;
  } else {
    memcpy(req_page->data, data, DEV_BLOCK_SIZE);
    return BLOCK_OK;
  }
  // Logic error.
  return BLOCK_FAIL;
}

block_rtvl_t block_free(int dev, block_t block)
{
  // RGDTODO - I don't think we need this anymore.
  if(dev < 0 || block < 0) {
    return BLOCK_PARAM;
  }
  return BLOCK_OK;
}

block_rtvl_t block_sync(int dev)
{
  if(dev < 0) {
    return BLOCK_PARAM;
  }

  if(!block_dev[dev]) {
    return BLOCK_DEV;
  }

  if(!block_hash[dev]) {
    return BLOCK_HASH;
  }

  queue_t *queue = block_dev[dev];
  hash_t  *hash  = block_hash[dev];
  // Write all blocks to disk.
  return block_sync_queue(queue);
}

block_rtvl_t block_close(int dev)
{
  bool error = false;
  if(block_sync(dev) != BLOCK_OK) {
    error = true;
  }
  if(dev_close(dev) != DEV_OK) {
    error = true;
  }
  if(!error) {
    block_free_queue(block_dev[dev]);
    block_dev[dev] = NULL;
    block_free_hash(block_hash[dev]);
    block_hash[dev] = NULL;
  }
  if(error) {
    return BLOCK_FAIL;
  }
  return BLOCK_OK;
}

void block_reinit()
{
  // See inode.c, when initializing the fs, we call block_open
  // twice, once to see if we have a fs, and again after inode_mkfs,
  // the second time we need to reinitialize our cache otherwise
  // we will read from a stale cache.
  init = false;
}

#ifdef _TEST_BLOCK
int main(int argc, char **argv)
{
  // This test checks the buffer logic in reading/writing the first three blocks
  // then writes 4096*3 blocks (purposefully greater than the number of
  // blocks in the cache).
  // It was verified that the cache successfully writes to the disk
  // all of the blocks and reads them ok.
  block_t i = 0;
  int dev = 0;
  char buf[DEV_BLOCK_SIZE]= {0};
  if(block_open("./block_cache.dat", &dev) != BLOCK_OK) {
    printf("error opening cache\n");
    return 1;
  }
  if(block_read(dev, 0, buf) != BLOCK_OK) {
    printf("error reading block <1>\n");
    return 1;
  }
  strcpy(buf, "hello world\n");
  if(block_write(dev, 0, buf) != BLOCK_OK) {
    printf("error writing block\n");
    return 1;
  }
  if(block_read(dev, 1, buf) != BLOCK_OK) {
    printf("error reading block <2>\n");
    return 1;
  }
  strcpy(buf, "hello world(2)\n");
  if(block_write(dev, 1, buf) != BLOCK_OK) {
    printf("error writing block\n");
    return 1;
  }
  if(block_read(dev, 2, buf) != BLOCK_OK) {
    printf("error reading block <3>\n");
    return 1;
  }
  strcpy(buf, "hello world(3)\n");
  if(block_write(dev, 2, buf) != BLOCK_OK) {
    printf("error writing block\n");
    return 1;
  }
  for(i = 3; i < (4096 * 3); ++i) {
    if(block_read(dev, i, buf) != BLOCK_OK) {
      printf("error reading block <4>\n");
      return 1;
    }
    sprintf(buf,"%ld\n",i);
    if(block_write(dev, i, buf) != BLOCK_OK) {
      printf("error writing block <4>\n");
      return 1;
    }
  }
  printf("write ok\n");
  for(i = 3; i < (4096 * 3); ++i) {
    if(block_read(dev, i, buf) != BLOCK_OK) {
      printf("error reading block<5>\n");
      return 1;
    }
    if(strtoul(buf, NULL, 10) != i) {
      printf("error reading block<5>\n");
    }
  }
  printf("read ok\n");
  if(block_close(dev) != BLOCK_OK) {
    printf("error closing cache\n");
    return 1;
  }
  return 0;
}
#endif
