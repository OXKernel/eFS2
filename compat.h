#ifndef _COMPAT_H
#define _COMPAT_H

#define P() printf("file=%s line=%d\n",__FILE__,__LINE__)

#include "types.h"
#include "errno.h"

// Macros for UNIX permissions (from sys/stat.h)
#define S_IFMT   00170000
#define S_IFREG   0100000  /* standard file */
#define S_IFBLK   0060000  /* block special */
#define S_IFDIR   0040000  /* directory */
#define S_IFCHR   0020000  /* character special */
#define S_IFIFO   0010000  /* pipe/fifo */
#define S_ISUID   0004000  /* user id  set on exe */
#define S_ISGID   0002000  /* group id set on exe */
#define S_ISVTX   0001000  /* save swapped text */
#define S_IFLNK   0120000 /* Symbolic link.  */
#define S_IFSOCK  0140000 /* Socket.  */

/* POSIX Sec 5.6.1.1 */
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG) /* is regular file */
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR) /* is directory */
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR) /* is char special */
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK) /* is block special */
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO) /* is pipe/fifo */

/* POSIX st_mode masks */
/* user */
#define S_IRWXU 00700  /* rwx------ */
#define S_IRUSR 00400  /* r-------- */
#define S_IWUSR 00200  /* -w------- */
#define S_IXUSR 00100  /* --x------ */

/* group */
#define S_IRWXG 00070  /* ---rwx--- */
#define S_IRGRP 00040  /* ---r----- */
#define S_IWGRP 00020  /* ----w---- */
#define S_IXGRP 00010  /* -----x--- */

/* other */
#define S_IRWXO 00007  /* ------rwx */
#define S_IROTH 00004  /* ------r-- */
#define S_IWOTH 00002  /* -------w- */
#define S_IXOTH 00001  /* --------x */

// Include all values of errno (see if we can get it from ox or minix.

#define O_RDONLY	     01 /* Was 00 but this wont work for bit operations. */
#define O_WRONLY	     02
#define O_RDWR		     04
#define O_CREAT		   0100	/* not fcntl */
#define O_TRUNC		  01000	/* not fcntl */
#define O_APPEND	  02000

/* We should have a sys/stat.h in ox.
 */
struct stat {
  dev_t     st_dev;     /* ID of device containing file */
  ino_t     st_ino;     /* inode number */
  mode_t    st_mode;    /* protection */
  nlink_t   st_nlink;   /* number of hard links */
  uid_t     st_uid;     /* user ID of owner */
  gid_t     st_gid;     /* group ID of owner */
  dev_t     st_rdev;    /* device ID (if special file) */
  off_t     st_size;    /* total size, in bytes */
  blksize_t st_blksize; /* blocksize for file system I/O */
  blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
  time_t    st_atime;   /* time of last access */
  time_t    st_mtime;   /* time of last modification */
  time_t    st_ctime;   /* time of last status change */
};

struct dirent
{
#ifndef __USE_FILE_OFFSET64
  ino_t d_ino;
  off_t d_off;
#else
  ino64_t d_ino;
  off64_t d_off;
#endif
  unsigned short int d_reclen;
  unsigned char d_type;
  char d_name[MAX_PATH];
};

struct __dirstream
{
  int     __fd;	            /* File descriptor. */
  inode_t __data;	            /* Directory block. */
  size_t  __allocation;	    /* Space allocated for the block. */
  size_t  __offset;	        /* Current offset into the block. */
  size_t  __block_ptr;	    /* The current block we are in.   */
  block_t __block;            /* Actual block number. */
  char    __path[MAX_PATH];   /* path to this dir.    */
  struct dirent __entry;      /* Filled by readdir.   */
};

typedef struct __dirstream DIR;

// From utime.h
struct utimbuf {
  time_t actime;   // Access time.
  time_t modtime;  // Modified time.
};

// In ox kernel, this will be available.
// Provided here for testing.
typedef struct proc {
  block_t         cwd;
  inode_perm_t    umask;
  inode_own_t     owner;
  inode_group_t   group;
  inode_t file_tab[MAX_FILES];
  inode_t *file_desc[MAX_FILES];
  DIR dir_tab[MAX_DIR];
} proc_t;

static proc_t proc = {0};
static proc_t *current_process = &proc;

// Return number of seconds since the epoch.
extern time_t time(time_t *t);

#define SSIZE_MAX 2147483647

#endif
