//
// @file:
//      types.h
//
// @description:
//      UNIX types for compilation.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _TYPES_H
#define _TYPES_H

typedef int dev_t;
typedef unsigned int ino_t;   // Should match block_t in block.h; defined in stdio.h
typedef unsigned int mode_t;  // Should match inode_perm_t in inode.h
typedef unsigned int nlink_t; // Should match block_t in block.h
typedef unsigned int uid_t;   // Should match inode_own_t in inode.h
typedef unsigned int gid_t;   // Should match inode_group_t in inode.h
// typedef unsigned long long int off_t;   // Should match inode_ptr_t in inode.h; defined in stdio.h
typedef int blksize_t; // Should be DEV_BLOCK_SIZE (512B)
typedef unsigned int blkcnt_t;  // Should match block_t in block.h
typedef unsigned int time_t;
typedef unsigned long long int ino64_t; // Should match inode_ptr_t in inode.h
typedef unsigned long long int off64_t; // Should match inode_ptr_t in inode.h
typedef unsigned int mode_t; // From unix, usually __U32_TYPE see /usr/include/bits/typesizes.h

#endif
