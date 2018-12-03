/*
    Name:           ID:
    Carlos Meza     1001170229
    Matthew Crum    1001077895   

*/

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)      ((b) + 1)
#define BLOCK_HEADER(ptr)   ((struct block *)(ptr) - 1)


static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct block 
{
   size_t      size;  /* Size of the allocated block of memory in bytes */
   struct block *next;  /* Pointer to the next block of allcated memory   */
   bool        free;  /* Is this block free?                     */
};


struct block *FreeList = NULL; /* Free list to track the blocks available */

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free blocks
 * \param size size of the block needed in bytes 
 *
 * \return a block that fits the request or NULL if no free block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */
struct block *findFreeBlock(struct block **last, size_t size) 
{
   struct block *curr = FreeList;
   
#if defined FIT && FIT == 0
   /* First fit */
   while (curr && !(curr->free && curr->size >= size)) 
   {
      *last = curr;
      curr  = curr->next;    
      
   }
#endif

#if defined BEST && BEST == 0
   /* Best Fit */
   size_t best=0;
   while (curr)
   {
      if ( curr->free && curr->size >= size)
      {
         if (best == 0 || curr->size < best) 
         {
            best = curr->size;

         }
      }
      *last = curr;
      curr = curr->next;
   
      }
    struct block *temp = FreeList;
   while(temp)
   {
      if (temp->size == best) 
      {
         curr = temp; 
         break;
      }
      temp = temp->next;
   }


#endif

#if defined WORST && WORST == 0
   /* Worst fit */
   size_t max = 0;
   while(curr)
   {
      if(curr->free && curr->size >= size)
      {
         if (max == 0 || curr->size > max)
         {
            max = curr->size;
         }
      }   
      *last = curr;
      curr = curr->next;
   }
   struct block *temp = FreeList;
   while (temp)
   {
      if (temp->size == max)
      {
         curr = temp;
         break;
      }
      temp = temp->next;
   }

#endif

#if defined NEXT && NEXT == 0
   /* Next fit */
   while (curr && !(curr->free && curr->size >= size))
   {
      *last = curr;
      FreeList = curr;
      curr = curr->next;
   }

#endif

   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated block of NULL if failed
 */
struct block *growHeap(struct block *last, size_t size) 
{
   /* Request more space from OS */
   struct block *curr = (struct block *)sbrk(0);
   struct block *prev = (struct block *)sbrk(sizeof(struct block) + size);
   
   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct block *)-1) 
   {
      return NULL;
   }

   /* Update FreeList if not set */
   if (FreeList == NULL) 
   {
      FreeList = curr;
   }

   /* Attach new block to prev block */
   if (last) 
   {
      last->next = curr;
   }

   /* Update block metadata */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;

   num_grows += 1;
   return curr;
}
/* 
 *
 * Function to split free block
 * 
 * */
/*void split(block *b, size_t size) 
{
   block *last, *next;
   last = (block *) (((char *) b) + s + sizeof(block));
   last->size = b->size - size - sizeof(block);
   // work in progress
}*/
/*
 * \brief malloc
 *
 * finds a free block of heap memory for the calling process.
 * if there is no free block that satisfies the request then grows the 
 * heap and returns a new block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{
   /* Count total memory requested */
   num_requested += size;
   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0) 
   {
      return NULL;
   }

   /* Look for free block */
   struct block *last = FreeList;
   struct block *next = findFreeBlock(&last, size);

   /* TODO: Split free block if possible */
   //size_t extra = next->size - size;
/*if (next->size > size)
   {
    //  next->size = size;
      num_splits += 1;
      //next->next->size += extra;
   }
   //split(next,size);
   */
   /* Could not find free block, so grow heap */
   if (next == NULL) 
   {
      next = growHeap(last, size);
   }

   /* Could not find free block or grow heap, so just return NULL */
   if (next == NULL) 
   {
      return NULL;
   }
   
   /* Mark block as in use */
   next->free = false;
   
   /* Count mallocs */
   num_mallocs += 1;

   /* Return data address associated with block */
   return BLOCK_DATA(next);
}

/*
 * \brief free
 *
 * frees the memory block pointed to by pointer. if the block is adjacent
 * to another block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr) 
{
   if (ptr == NULL) 
   {
      return;
   }

   /* Make block as free */
   struct block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;
   
   /* Count frees */
   num_frees += 1;

   /* TODO: Coalesce free blocks if needed */
 
  struct block *succ = FreeList;
   if (curr->next != NULL) 
      succ = curr->next;
   if (curr->free && succ->free && succ != NULL)
   {
      curr->size += succ->size;
      succ = succ->next;
      curr->next = succ;
      num_coalesces += 1;  
   }
}

/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
