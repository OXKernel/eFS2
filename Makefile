CFLAGS=-D_TEST_INODE_INC -D_TEST_BLOCK_INC -D_TEST_DEV_INC -D_TEST_DIR_INC -D_TEST_FILE_INC -D_TEST_LINK_INC -D_TEST_PATHS_INC
ALL: inode.o block.o dev.o paths.o
	gcc -m32 -g -c dev.c
	gcc -m32 -g -D_TEST_BLOCK -D_TEST_BLOCK_INC block.c lru.c dev.o -o block
	gcc -m32 -g -D_TEST_INODE_INC -D_TEST_INODE -D_TEST_BLOCK_INC -D_TEST_PATHS_INC -D_TEST_KREALPATH_WITH_INODE -D_TEST_LINK_INC inode.c block.c lru.c paths.c bitmap.c krealpath.c compat.c link.c dev.o -o inode
	gcc -m32 -g -D_TEST_INODE_INC -D_TEST_BLOCK_INC -D_TEST_PATHS_INC -D_TEST_KREALPATH_WITH_INODE -D_TEST_LINK_INC -D_TEST_FILE -D_TEST_FILE_INC -D_TEST_DIR_INC inode.c block.c lru.c paths.c bitmap.c krealpath.c compat.c link.c file.c dir.c dev.o -o file
	gcc -m32 -g -D_TEST_INODE_INC -D_TEST_BLOCK_INC -D_TEST_PATHS_INC -D_TEST_KREALPATH_WITH_INODE -D_TEST_LINK_INC -D_TEST_FILE_INC -D_TEST_DIR_INC -D_TEST_DIR inode.c block.c lru.c paths.c bitmap.c krealpath.c compat.c link.c file.c dir.c dev.o -o dir
	gcc -m32 -g -D_TEST_INODE_INC -D_TEST_BLOCK_INC -D_TEST_PATHS_INC -D_TEST_KREALPATH_WITH_INODE -D_TEST_FILE_INC -D_TEST_DIR_INC -D_TEST_LINK_INC -D_TEST_LINK inode.c block.c lru.c paths.c bitmap.c krealpath.c compat.c file.c dir.c link.c dev.o -o link

krealpath: krealpath.o paths.o
	gcc -m32 -D_TEST_KREALPATH -g -c krealpath.c
	gcc -m32 -D_TEST_PATHS_INC -g -c paths.c
	gcc -m32 -o krealpath paths.o krealpath.o

clean:
	rm -f *.o block inode file dir link block_cache.dat krealpath inode.dat
