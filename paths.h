//
// @file:
//      paths.h
//
// @description:
//      Utility routines for handling paths.
//
// @author:
//      Dr. Roger G. Doss
//
#ifndef _PATHS_H
#define _PATHS_H

#define MAX_PATH           256
#define MAX_FILES          1024
#define MAX_DIR            1024

//
// push_front:
// pre-pend src in front of dst, return
// results in dst. Return NULL if length greater
// than MAX_PATH.
//
char *push_front(char *dst, char *src);

// get_next_path:
//  Given an integer start, obtain the next path.
//  Used in inode.c.
char *get_next_path(int *start, char *path);

// get_parent_dir:
//  Get the parent directory of the path.
void get_parent_dir(char *path, char *parent);

// get_parent_path:
//  Get the parent path of the path.
void get_parent_path(char *path, char *parent);

#endif
