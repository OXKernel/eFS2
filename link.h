//
// @file:
//      link.h
//
// @description:
//      Implementation of file system symlinks
//      and hardlinks.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _LINK_H
#define _LINK_H

#define MAX_RECURSION 1024
int klink(const char *oldpath, const char *newpath);
block_t kreadlink(int dev, int current_dir, char *pathcomponent, int *error);

#endif // _LINK_H
