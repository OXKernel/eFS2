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
#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "block.h"
#include "lru.h"

// A utility function to create a new Queue Node. The queue Node
// will store the given 'pageNumber'
qnode_t* new_qnode(unsigned page_number)
{
  // Allocate memory and assign 'pageNumber'
  qnode_t* temp = (qnode_t*)calloc(1, sizeof(qnode_t));
  temp->page_number = page_number;

  // Initialize prev and next as NULL
  temp->prev = temp->next = NULL;

  return temp;
}

// A utility function to create an empty Queue.
// The queue can have at most 'numberOfFrames' nodes
queue_t* create_queue(int dev, int number_of_frames)
{
  queue_t* queue = (queue_t*)calloc(1, sizeof(queue_t));

  queue->dev = dev;
  // The queue is empty
  queue->count = 0;
  queue->front = queue->rear = NULL;

  // Number of frames that can be stored in memory
  queue->number_of_frames = number_of_frames;

  return queue;
}

// A utility function to create an empty Hash of given capacity
hash_t* create_hash(int capacity)
{
  // Allocate memory for hash
  hash_t* hash = (hash_t*)calloc(1, sizeof(hash_t));
  hash->capacity = capacity;

  // Create an array of pointers for referring queue nodes
  hash->array = (qnode_t**)calloc(1, hash->capacity * sizeof(qnode_t*));

  // Initialize all hash entries as empty
  int i;
  for (i = 0; i < hash->capacity; ++i) {
    hash->array[i] = NULL;
  }

  return hash;
}

// A function to check if there is slot available in memory
int are_all_frames_full(queue_t* queue)
{
  return queue->count == queue->number_of_frames;
}

// A utility function to check if queue is empty
int is_queue_empty(queue_t* queue)
{
  return queue->rear == NULL;
}

// A utility function to delete a frame from queue
void dequeue(queue_t* queue)
{
  if (is_queue_empty(queue)) {
    return;
  }


  // If this is the only node in list, then change front
  if (queue->front == queue->rear) {
    queue->front = NULL;
  }

  // Change rear and remove the previous rear
  qnode_t* temp = queue->rear;
  queue->rear = queue->rear->prev;

  if (queue->rear) {
    queue->rear->next = NULL;
  }

  // Write to drive
  block_disk_write(queue->dev, temp->page_number, temp->data); // FIXME make sure we check the return value here.
  // Free memory
  free(temp);

  // decrement the number of full frames by 1
  queue->count--;
}

// A function to add a page with given 'pageNumber' to both queue
// and hash
block_rtvl_t enqueue(queue_t* queue, hash_t* hash, unsigned page_number)
{
  // If all frames are full, remove the page at the rear
  int pos = -1;
  if (are_all_frames_full(queue)) {
    // remove page from hash
    qnode_t *node = find_in_hash(hash, &pos, queue->rear->page_number);
    if(!node) {
      return BLOCK_LOGIC;
    }
    // Remove from hash.
    hash->array[pos] = NULL; // deQueue will remove from the rear, so we are good as it will write to disk.
    dequeue(queue);
  }

  // Create a new node with given page number,
  // And add the new node to the front of queue
  qnode_t* temp = new_qnode(page_number);
  temp->next = queue->front;

  // If queue is empty, change both front and rear pointers
  if (is_queue_empty(queue)) {
    queue->rear = queue->front = temp;
  } else { // Else change the front
    queue->front->prev = temp;
    queue->front = temp;
  }

  // Add page entry to hash also
  if(pos != -1) {
    hash->array[pos] = temp; // Insert into the free slot.
  } else {
    // If queue was not full prior, we should be able
    // to find a slot.
    if(insert_in_hash(hash, temp, temp->page_number) == -1) {
      return BLOCK_LOGIC; // This is a logic error, it means the queue wasn't full but the hash was.
    }
  }

  // increment number of full frames
  queue->count++;

  if(queue->rear)
    // Read from disk.
    block_disk_read(queue->dev, temp->page_number, temp->data);  // FIXME Check return values here.
  return BLOCK_OK;
}

int insert_in_hash(hash_t *hash, qnode_t *node, int page_number)
{
  register int i = 0;
  register int index = page_number % hash->capacity;
  for(i = index; i < hash->capacity; ++i) {
    if(hash->array[i] == NULL) {
      hash->array[i] = node;
      return i; // Return slot we inserted in.
    }
  }
  for(i = 0; i < index; ++i) {
    if(hash->array[i] == NULL) {
      hash->array[i] = node;
      return i; // Return slot we inserted in.
    }
  }
  return -1; // No slot.
}

qnode_t *find_in_hash(hash_t *hash, int *position, int page_number)
{
  register int i = 0;
  register int index = page_number % hash->capacity;
  bool found = false;
  qnode_t* req_page = hash->array[page_number % hash->capacity];

  for(i = index; i < hash->capacity; ++i) {
    req_page = hash->array[i];
    if(req_page && req_page->page_number == page_number) {
      *position = i;
      found = true;
      break;
    }
  }
  if(!found) {
    for(i = 0; i < index; ++i) {
      req_page = hash->array[i];
      if(req_page && req_page->page_number == page_number) {
        *position = i;
        found = true;
        break;
      }
    }
  }
  if(!found) {
    *position = -1;
    return NULL;
  }
  return req_page;
}

// This function is called when a page with given 'pageNumber' is referenced
// from cache (or memory). There are two cases:
// 1. Frame is not there in memory, we bring it in memory and add to the front
// of queue
// 2. Frame is there in memory, we move the frame to front of queue
block_rtvl_t reference_page(queue_t* queue, hash_t* hash, unsigned page_number)
{
  int pos = 0;
  qnode_t* req_page = find_in_hash(hash, &pos, page_number);
  block_rtvl_t rtvl = BLOCK_OK;

  if (req_page == NULL) {
    if(enqueue(queue, hash, page_number) != BLOCK_OK) {
      return BLOCK_LOGIC;
    }
  }
  // page is there and not at front, change pointer
  else if (req_page != queue->front) {
    // Unlink rquested page from its current location
    // in queue.
    if(req_page->prev) {
      req_page->prev->next = req_page->next;
    }
    if (req_page->next) {
      req_page->next->prev = req_page->prev;
    }

    // If the requested page is rear, then change rear
    // as this node will be moved to front
    if (req_page == queue->rear) {
      queue->rear = req_page->prev;
      queue->rear->next = NULL;
    }

    // Put the requested page before current front
    req_page->next = queue->front;
    req_page->prev = NULL;

    // Change prev of current front
    req_page->next->prev = req_page;

    // Change front to the requested page
    queue->front = req_page;
  }
  return BLOCK_OK;
}

// Block IO
block_rtvl_t block_sync_queue(queue_t *queue)
{
  qnode_t *ptr = queue->front;
  block_rtvl_t rtvl = BLOCK_OK;
  //int count  = 0;
  // For entire queue, write all blocks to disk to sync.
  //for(; count < queue->count && ptr; ptr = ptr->next, count++) {
  for(; ptr != queue->rear; ptr = ptr->next) {
    if((rtvl=block_disk_write(queue->dev, ptr->page_number, ptr->data)) != BLOCK_OK) {
      return rtvl;
    }
  }
  return BLOCK_OK;
}

block_rtvl_t block_disk_read(int dev, block_t block, char *data)
{
  if(dev_scan(dev,block) != DEV_OK) {
    return BLOCK_FAIL;
  }
  if(dev_read(dev, block, data) != DEV_OK) {
    return BLOCK_FAIL;
  }
}

block_rtvl_t block_disk_write(int dev, block_t block, char *data)
{
  if(dev_scan(dev,block) != DEV_OK) {
    return BLOCK_FAIL;
  }
  if(dev_write(dev, block, data) != DEV_OK) {
    return BLOCK_FAIL;
  }
  return BLOCK_OK;
}

block_rtvl_t block_free_queue(queue_t *queue)
{
  while(!is_queue_empty(queue)) {
    dequeue(queue);
  }
  free(queue);
  return BLOCK_OK;
}

block_rtvl_t block_free_hash(hash_t *hash)
{
  register int i = 0;
  for (; i < hash->capacity; ++i) {
    hash->array[i] = NULL;
  }
  free(hash);
  return BLOCK_OK;
}

#ifdef _TEST_LRU
// Driver program to test above functions
int main()
{
  // Let cache can hold 4 pages
  queue_t* q = create_queue(/*dev*/0, /*nrOfFrames*/4);

  // Let 10 different pages can be requested (pages to be
  // referenced are numbered from 0 to 9
  hash_t* hash = create_hash(10);

  // Let us refer pages 1, 2, 3, 1, 4, 5
  reference_page(q, hash, 1);
  reference_page(q, hash, 2);
  reference_page(q, hash, 3);
  reference_page(q, hash, 1);
  reference_page(q, hash, 4);
  reference_page(q, hash, 5);

  // Let us print cache frames after the above referenced pages
  printf("%d ", q->front->page_number);
  printf("%d ", q->front->next->page_number);
  printf("%d ", q->front->next->next->page_number);
  printf("%d ", q->front->next->next->next->page_number);

  return 0;
}
#endif
