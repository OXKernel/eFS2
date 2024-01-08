//
// @file:
//      dir.h
//
// @description:
//      Implementation of directory specific functionality.
//      This is based on our inode implementation.
//
// @author:
//      Dr. Roger Doss, PhD
//
#ifndef _DIR_H
#define _DIR_H

DIR *kopendir(const char *path);
int kclosedir(DIR *dir);
void krewinddir(DIR *dir);
struct dirent *kreaddir(DIR *dir);
int krmdir(const char *path);
int kmkdir(const char *path, mode_t mode);

#endif // _DIR_H
