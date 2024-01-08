typedef block_t block_t;
#if 0
typedef struct master_inode {
  int init_fs;           /* 1 if fs is initialized, 0 otherwise. */
  int inode_map_start;   /* map of free inodes start. */
  int inode_map_end;     /* map of free inodes end.   */
  int bmap_map_start;    /* map of free blocks start. */
  int bmap_map_end;      /* map of free blocks end.   */
  int inode_start;       /* start of inodes.          */
  int inode_end;         /* end of inodes.            */
  int imap_ptr;          /* what inode map block on disk.  */
  int bmap_ptr;          /* what bit map block on disk.    */
  char *imap;            /* Created on the fly and loaded. */
  char *bmap;            /* Created on the fly and loaded. */
} master_inode_t;
#endif

typedef struct master_inode {
  short magic;           /* Magic to identify the file system. */
  short init_fs;         /* 1 if fs is initialized, 0 otherwise. */
  block_t inode_map_start;   /* map of free inodes start. */
  block_t inode_map_end;     /* map of free inodes end.   */
  block_t bmap_map_start;    /* map of free blocks start. */
  block_t bmap_map_end;      /* map of free blocks end.   */
  block_t inode_start;       /* start of inodes.          */
  block_t inode_end;         /* end of inodes.            */
  block_t data_start;        /* start of data.            */
  block_t data_end;          /* end of data.              */
  block_t imap_ptr;          /* what inode map block on disk.  */
  block_t imap_bit;          /* what inode map bit. */
  block_t bmap_ptr;          /* what bit map block on disk.    */
  block_t bmap_bit;          /* what bit map bit. */
  char *imap;            /* Created on the fly and loaded. */
  char *bmap;            /* Created on the fly and loaded. */
  block_t dev;      /* Device we are on. */
  char pad[448];   /* padding to fit page size. */
} master_inode_t; // RGDTODO - Are we still 512 bytes ?

typedef struct inode {
  char user_read;
  char user_write;
  char user_execute;

  char group_read;
  char group_write;
  char group_execute;

  char world_read;
  char world_write;
  char world_execute;

  char is_directory; /* If this is a directory,
                       * all blocks point to other inodes, including next. */
  char is_device;    /* If this is a device, call device driver subsystem. */
  char is_file;      /* If this is a file, all blocks point to data, including next. */

  int  create_time;
  int  modified_time;
  int  accessed_time;

  int  self;
  int  parent;

  char name[256];
  int  blocks[55]; // This would make 512 bytes.
  int next; /* Map to blocks in case of a regular file. */

} inode_t;

int
main()
{
  printf("%d\n",sizeof(master_inode_t));
  printf("%d\n",sizeof(inode_t));
  return 0;
}
