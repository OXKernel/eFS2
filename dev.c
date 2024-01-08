//
// @file:
//      dev.c
//
// @description:
//      Low level device interface. For testing in user space,
//      we read/write in sector blocks from a top a file system.
//      In kernel mode, we make the actual calls to a hard disk
//      via a driver.
//
//      NOTE: This code was tested as part of testing block.c.
//
// @author:
//      Dr. Roger Doss, PhD
//
#include "block.h"
#include "dev.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

dev_rtvl_t dev_open(char *path, int *dev)
{
  if(!path || !dev) {
    return DEV_PARAM;
  }
  (*dev) = open(path, O_RDWR | O_CREAT, S_IRWXU);
  if(*dev < 0) {
    *dev = DEV_FAIL;
    return DEV_FAIL;
  }
  return DEV_OK;
}

dev_rtvl_t dev_close(int dev)
{
  if(dev < 0) {
    return DEV_PARAM;
  }
  close(dev);
}

dev_rtvl_t dev_read(int dev, block_t block, char *data)
{
  if(dev < 0 || !data) {
    return DEV_PARAM;
  }
  if(read(dev, data, DEV_BLOCK_SIZE) < 0) {
    return DEV_FAIL;
  }
  return DEV_OK;
}

dev_rtvl_t dev_write(int dev, block_t block, char *data)
{
  if(dev < 0 || !data) {
    return DEV_PARAM;
  }
  if(write(dev, data, DEV_BLOCK_SIZE) < 0) {
    return DEV_FAIL;
  }
  return DEV_OK;
}

dev_rtvl_t dev_scan(int dev, block_t block)
{
  if(dev < 0 || block < 0) {
    return DEV_PARAM;
  }
  if(lseek(dev, block * DEV_BLOCK_SIZE, 0) < 0) {
    return DEV_FAIL;
  }
  return DEV_OK;
}

