// 
// @file:
//  lru.h
//
// @description:
//  Implements LRU cache for eFS.
//
// @author:
//  Dr. Roger Doss
//
// REFERENCES:
// https://www.geeksforgeeks.org/lru-cache-implementation/
//
#ifndef _LRU_H
#define _LRU_H 1

#include "dev.h"
#include "block.h"

typedef struct qnode {
  struct qnode *prev, *next;
  block_t page_number;         // the page number stored in this QNode RGDTODO Should be block_t
  char data[DEV_BLOCK_SIZE];   // the data associated with this node
} qnode_t;

typedef struct queue {
  unsigned count;             // Number of filled frames
  unsigned number_of_frames;  // total number of frames
  qnode_t *front, *rear;
  int dev;                    // Device we are on.
} queue_t;

typedef struct hash {
  int capacity;       // how many pages can be there
  qnode_t** array;    // an array of queue nodes
} hash_t;

// creators:
qnode_t* new_qnode(unsigned page_number);

queue_t* create_queue(int dev, int number_of_frames);

// hash utility functions:
hash_t* create_hash(int capacity);

int insert_in_hash(hash_t *hash, qnode_t *node, int page_number);

qnode_t *find_in_hash(hash_t *hash, int *position, int page_number);

// queue utility functions:
int are_all_frames_full(queue_t* queue);

int is_queue_empty(queue_t* queue);

void dequeue(queue_t* queue);

block_rtvl_t enqueue(queue_t* queue, hash_t* hash, unsigned page_number);

// This function is called when a page with given 'pageNumber' is referenced
// from cache (or memory). There are two cases:
// 1. Frame is not there in memory, we bring it in memory and add to the front
// of queue
// 2. Frame is there in memory, we move the frame to front of queue
block_rtvl_t reference_page(queue_t* queue, hash_t* hash, unsigned page_number);

// disk I/O routines:
block_rtvl_t block_sync_queue(queue_t *queue);

block_rtvl_t block_disk_read(int dev, block_t block, char *data);

block_rtvl_t block_disk_write(int dev, block_t block, char *data);

block_rtvl_t block_free_queue(queue_t *queue);

block_rtvl_t block_free_hash(hash_t *hash);

#endif // _LRU_H
