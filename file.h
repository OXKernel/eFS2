//
// @file:
//      file.h
//
// @description:
//      Implementation of file specific functionality.
//      This is based on our inode implementation.
//
// @author:
//      Dr. Roger Doss, PhD
//
#ifndef _FILE_H
#define _FILE_H

typedef enum file_rtvl {
  FILE_OK     = 0,
  FILE_FAIL   = -1
} file_rtvl_t;

file_rtvl_t file_add_blocks(int dev,
                            block_t *next,
                            block_t *current,
                            int     *iblock,
                            block_t *bmap_block,
                            int nr_blocks,
                            char *data,
                            inode_ptr_t length);

file_rtvl_t file_add_one_bmap_block(int dev,
                                    block_t current,
                                    block_map_t *bmap);

file_rtvl_t file_add_one_data_block(int dev,
                                    block_t current,
                                    block_map_t *bmap,
                                    block_t iblock);

file_rtvl_t file_read_write(inode_t *inode,
                            inode_ptr_t pos,
                            inode_ptr_t length,
                            char *data,
                            bool reading,
                            inode_ptr_t *bytes_processed);

int kopen(const char *path, int flags);
int kopen2(const char *path, int flags, mode_t mode);
int kcreat(const char *path, mode_t mode);
int kunlink(const char *path);
int kclose(int fd);
ssize_t kread(int fd, void *buf, size_t count);
ssize_t kwrite(int fd, const void *buf, size_t count);
off_t klseek(int fd, off_t offset, int whence);
int kdup(int fd);
int kdup2(int fd, int newfd);
int kstat(const char *path, struct stat *buf);
int kfstat(int fd, struct stat *buf);
int klstat(const char *path, struct stat *buf);
void ksync(void);
int kchmod(const char *path, mode_t mode);
int kfchmod(int fd, mode_t mode);
char *kgetcwd(char *buf, size_t size);
char *kgetwd(char *buf);
char *kget_current_dir_name(void);
int kchdir(const char *path);
int kfchdir(int fd);
int kutime(const char *filename, struct utimbuf *buf);
int krename(const char *oldpath, const char *newpath);

#endif // _FILE_H
