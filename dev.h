//
// @file:
//      dev.h
//
// @description:
//      Low level device interface. For testing in user space,
//      we read/write in sector blocks from a top a file system.
//      In kernel mode, we make the actual calls to a hard disk
//      via a driver.
//
// @author:
//      Dr. Roger Doss, PhD
//
#ifndef _DEV_H
#define _DEV_H

// Size of a disk sector in bytes.
// NOTE: There are new developments for 4096 byte sectors.
// For now, we will keep this inline with traditional 512 byte
// sectors inline with boot loader code.
// See: http://www.ibm.com/developerworks/linux/library/l-4kb-sector-disks/
#define DEV_BLOCK_SIZE 512
#define DEV_NODEV -1

typedef enum dev_rtvl {
  DEV_OK      = 0,
  DEV_FAIL    = -1,
  DEV_PARAM   = -2
} dev_rtvl_t;

dev_rtvl_t dev_open(char *path, int *dev);

dev_rtvl_t dev_close(int dev);

//
// dev_read:
// dev_write:
//
// We now include the block number in case it is needed
// when we actually make the kernel device read/write.
//
dev_rtvl_t dev_read(int dev, block_t block, char *data);

dev_rtvl_t dev_write(int dev, block_t block, char *data);

dev_rtvl_t dev_scan(int dev, block_t block);

#endif
