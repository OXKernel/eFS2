//
// @file:
//      krealpath.h
// @description:
//      Implements kernel's realpath resolution.
//      This resolves a relative path into an absolute
//      path given a starting current_dir.
//
// @author:
//      Dr. Roger Doss, PhD
//
#ifndef _KREALPATH_H
#define _KREALPATH_H

//
// krealpath:
//  dev = device we are running on
//  current_dir = block representing current directory
//  path =  the path to resolve
//  res_path = the resulting path from this function
//  error = the errno error value
//
//  NOTE: path and res_path are allocated outside of this function
//  and are normall just stack based arrays. They must be
//  MAX_PATH in length and the resulting path must also be
//  MAX_PATH in length.
//
//  Function returns NULL in the event of an error.
//
char *krealpath(int dev, block_t current_dir, char *path, char *res_path, int *error);

#endif
