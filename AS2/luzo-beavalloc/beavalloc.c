#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "beavalloc.h"

struct block_list{
	size_t size;
	size_t capacity;
	struct block_list* prev;
	struct block_list* next;
	int free;
	int in_used;
	void *data;
};

struct block_list* head = NULL;
// struct block_list* tail = NULL;
// struct block_list* flag;

int has_setup = 0;
static int has_initialized = 0; /* 初始化标记 */
static void *managed_memory_start; /* 管理内存起始地址 */
void *last_valid_address; /* 操作系统的最后一个有效地址*/


void* memory_location;


// int* global_base = ptr;
void *lower_mem_bound = 0;
void *upper_mem_bound = 0;

// static void *first_base = NULL;

uint8_t verbose_flag;

void initialize(void);
struct block_list *get_block(size_t);



void beavalloc_reset(void){
	has_initialized = 0;
	printf("what is head????? %p\n", head);
	//brk((void *)head - sizeof(struct block_list));
	brk(head);
	head = NULL;

	managed_memory_start = NULL;
	//brk(managed_memory_start);//reset to the initial heap locaiton
}


void initialize(void){
	last_valid_address = sbrk(0);
	lower_mem_bound = last_valid_address;
	managed_memory_start = last_valid_address;
	if(last_valid_address == (void *) (-1)){
		perror("sbrk");
		_exit(127);
	}
	upper_mem_bound = last_valid_address;
	has_initialized = 1;

}

struct block_list *get_block(size_t size){
	struct block_list *base = NULL;

  if (base == NULL)
   {
		 if(has_setup == 0){
			 // managed_memory_start = managed_memory_start + sizeof(struct block_list);
			 base = managed_memory_start;
			 // printf("In get_block: **********base: %p\n", base);
			 has_setup = 1;
		 }
		 else{
			 	// last_valid_address = last_valid_address + size;
				// printf("In get_block(): last_valid_address: %p\n", last_valid_address);
				// printf("does this work\n");
		 		base = last_valid_address;
				// printf("base address: %p\n", base);
		}
			// printf("last_valid_address: %p\n", last_valid_address);
    // if ((base = sbrk(word_align(sizeof(struct chunk)))) != (void *)-1)
    //  {
			base->size = size;
			base->capacity = 1024 - size - sizeof(struct block_list);
      base->next = NULL;
      base->prev = NULL;
      base->free = 0;
			base->in_used = 1;
			base->data = memory_location + sizeof(struct block_list);
			// printf("********base: %p\n", base);
			// printf("--------base->data: %p\n", base->data);
     // }
   }
  return base;
}

/*
// if (first_base == NULL)
// 	first_base = sbrk(0);
//
// 	if (!head) {
// 		head = sbrk(1024);
// 		head->prev = head->next = NULL;
// 		head->in_use = TRUE;
// 		head->free = FALSE;
// 		head->capacity = 1024 - sizeof(struct block_list);
// 		head->size = size;
// 	}
// 	else {
// 		struct block_list *curr = head;
//
// 		// check for free node or splittable node.
//
// 		for (; curr->next != NULL; curr=curr->next) {
// 			//check for splittable node
// 			if (curr->in_use == FALSE) {
//
// 			}
//
//
//
//
// 			curr->next = sbrk(1024);
// 			curr->next->prev = curr;
// 		}
// 	}
*/
void *beavalloc(size_t size){
	int reuse_flag = 0;
// #ifdef NOPE
struct block_list *curr_block = NULL;
struct block_list *box = NULL;
	memory_location = sbrk(0);

	if(verbose_flag == TRUE){
		printf("Verbose enabled\n");
		fprintf(stderr, ">> current memory_location = %p\n", memory_location);
	}
	if (size <= 0) {
		return NULL;
	}//end if

	if( has_initialized != 1){
		initialize();
		sbrk(size + sizeof(struct block_list));
		// last_valid_address = last_valid_address + size + sizeof(struct block_list);
		curr_block = get_block(size);
		// memory_location = last_valid_address;
		head = curr_block;
		printf("head: %p\n", head);
		head->next = NULL;
		upper_mem_bound = curr_block->data;
		// printf("In beavalloc: memory_location: %p\n", memory_location);
		memory_location = managed_memory_start + sizeof(struct block_list);
		last_valid_address = memory_location + size;
		// printf("@@@@@@@First time: memory_location: %p\n", memory_location);
	}//end if
	else{
		last_valid_address = memory_location;
		// printf("size: %d\n", size);
		box = head;

		// printf("box->size: %d\n", box->size);

		//getting to the end of double linked list
		while(box != NULL){
			// printf("^^^^^^^^^^current box address: %p^^^^^^^^^^\n", box);
			if((box->free == 1 && box->in_used == 0) && box->size >= size){
				reuse_flag = 1;
				break;
			}
			// printf("box->size: %d\n", box->size);
			if(box->next == NULL){
				break;
			}
			box = box->next;
		}//End while

		//Generally allocate new memory space
		if(reuse_flag == 0){
		// if(box->capacity < size){
			sbrk(size + sizeof(struct block_list));
		// }
			curr_block = get_block(size);

			box->next = curr_block;
			// printf("curr_block address: %p\n", curr_block);
			curr_block->prev = box;
			curr_block->next = NULL;

			// printf("box->next: %p\n", box->next);
		}
		//reuse space enable
		else{
			box->size = size;
			box->free = 0;
			box->in_used = 1;
		}
		// last_valid_address = curr_block->data + sizeof(struct block_list);
		// memory_location = memory_location + sizeof(struct block_list);
		upper_mem_bound = curr_block + size;
		memory_location = memory_location + sizeof(struct block_list);
		// memory_location = last_valid_address;
		// last_valid_address = last_valid_address + size;
		// last_valid_address = memory_location - sizeof(struct block_list);
		// printf("########memory_location: %p\n", memory_location);
		// printf("########last_valid_address: %p\n", last_valid_address);
		// memory_location = (void*)curr_block + sizeof(struct block_list);

	}//end else
	// memory_location = memory_location + sizeof(struct block_list);
	// printf("*******memory_location: %p\n", memory_location);
	return memory_location;
	// #endif // NOPR
}


void beavfree(void *p){
	struct block_list *cur_block = NULL;
	struct block_list *block_prev = NULL;
	struct block_list *block_next = NULL;
	if(!p)
		return;

	cur_block = (struct block_list*) (p - sizeof(struct block_list));
	if(verbose_flag == TRUE){
		printf("Verbose enabled\n");
		fprintf(stderr, ">> beavfree entry: ptr = %p curr = %p\n", p, cur_block);
	}
	// printf("head->NULL %p\n", head->prev);


	// printf("*p: %p\n", p);
	// struct block_list *cur_block = (struct block_list*) p;

	// struct block_list *cur_block = head;

	// struct block_list test = p;

	// while(cur_block->next != NULL){
	// 	if(cur_block == ptr){
	// 		break;
	// 	}
	// 	cur_block = cur_block->next;
	// }//end while

	// printf("cur_block %p\n", cur_block);
	if(verbose_flag == TRUE){
		if(cur_block->free == 1){
			fprintf(stderr, "<< Block is already free: ptr = %p\n", p);
			return;
	  }
	}
	cur_block->free = 1;
	cur_block->in_used = 0;

	block_prev = cur_block->prev;
	block_next = cur_block->next;

	//move to tail
	if(block_prev == NULL && block_next == NULL){
		// printf("broke here? if(block_prev == NULL && block_next == NULL)\n");
		return;
	}//end if statement
	else if(block_next == NULL && block_prev != NULL){
		if(block_prev->in_used == 0){

			block_prev->next = NULL;
			block_prev->size = cur_block->size + sizeof(struct block_list) + block_prev->size;
			// printf("broke here? if(block_prev->in_used == 0)\n");
			//
			// block_prev->next = cur_block->next;
			// block_next->prev = block_prev;
		}
	}//end else if statement
	else if(block_next != NULL && block_prev == NULL){
		if(block_next->in_used == 0){
			cur_block->next = block_next->next;
			cur_block->size = cur_block->size + sizeof(struct block_list) + block_next->size;
			if(block_next->next != NULL){
				block_next->next->prev = cur_block;
				// printf("broke here? if(block_next->next != NULL)\n");
			}
			// block_prev->size = cur_block->size + sizeof(struct block_list) + block_prev->size;
			// block_prev->next = cur_block->next;
			// block_next->prev = block_prev;
		}//end if statement
	}//end else if statement
	else if(block_prev != NULL && block_next != NULL){
		if(block_prev->in_used == 0 && block_next->in_used == 1){
			block_prev->size = cur_block->size + sizeof(struct block_list) + block_prev->size;
			block_prev->next = cur_block->next;
			block_next->prev = block_prev;
			// printf("broke here? if(block_prev->in_used == 0 && block_next->in_used == 1)\n");
		}//end if statement
		else if(block_prev->in_used == 1 && block_next->in_used == 0){
			// printf("************\n");
			cur_block->in_used = 0;
			// printf("&&&&&&&&&&&&\n");
			cur_block->size = cur_block->size + sizeof(struct block_list) + block_next->size;
			// printf("^^^^^^^^^^^^\n");
			cur_block->next = block_next->next;
			// printf("$$$$$$$$$$$$\n");
			if(block_next->next != NULL){
				block_next->next->prev = cur_block;
			}
			// printf("broke here? else if(block_prev->in_used == 1 && block_next->in_used == 0)\n");
		}//end else if statement
		else if(block_prev->in_used == 0 && block_next->in_used == 0){
			block_prev->size = block_prev->size + cur_block->size + block_next->size + sizeof(struct block_list);
			block_prev->next = block_next->next;
			if(block_next->next != NULL){
				block_next->next->prev = cur_block;
				// printf("broke here? if(block_next->next != NULL)\n");
			}
			else{
				block_next->prev = cur_block;
				// printf("broke here? else\n");
			}
		}//end else if statement
	}//end else if statement
	// struct block_list* box = head;
	// struct block_list* fix_box;
	//
	//
	// while(box->next != NULL){
	// 	fix_box = box->next;
	// 	if(box->free == 1 && fix_box->free == 1){
	// 		box->next = fix_box->next;
	// 		if(fix_box->next != NULL){
	// 		   box->next->prev = fix_box;
	// 		}
	// 	}
	// 	box = box->next;
	// }//end while
	// // head = box;
}


void beavalloc_set_verbose(uint8_t flag){
	verbose_flag = flag;
}


void *beavcalloc(size_t nmemb, size_t size){
	size_t length;
	void *ptr = NULL;
	if(verbose_flag == TRUE){
		printf("Verbose enabled\n");
		fprintf(stderr, ">> beavcalloc entry: nmemb = %zu size = %zu\n", nmemb, size);
	}
	if(nmemb == 0 || size == 0){
		return NULL;
	}
	length = nmemb * size;
	ptr = beavalloc(length);
	memset(ptr, 0, length);
	return ptr;
}



void *beavrealloc(void *ptr, size_t size){
	struct block_list *curr_ptr = (void *) ptr - sizeof(struct block_list);
	void *mem_block;
	size_t double_size = 2*size;

	if(verbose_flag == TRUE){
		printf("Verbose enabled\n");
		fprintf(stderr, ">> beavrealloc entry: ptr = %p size = %zu\n", ptr, size);
	}

	if(size <= 0 && ptr != NULL){
		beavfree(ptr);
		return NULL;
	}//end if
	else if(ptr == NULL && size > 0){
		mem_block = beavalloc(double_size);
	}//end else if
	else{
		if(curr_ptr->size < size){
			mem_block = beavalloc(size);
			memcpy(mem_block, ptr, curr_ptr->size);
		}
		else if(curr_ptr->size >= size){
			return ptr;
		}//end else if
	}//end else
	return mem_block;
}




void beavalloc_dump(uint leaks_only)
{
    struct block_list *curr = NULL;
    uint i = 0;
    uint leak_count = 0;
    uint user_bytes = 0;
    uint capacity_bytes = 0;
    uint block_bytes = 0;
    uint used_blocks = 0;
    uint free_blocks = 0;

    if (leaks_only) {
        fprintf(stderr, "heap lost blocks\n");
    }
    else {
        fprintf(stderr, "heap map\n");
    }
    fprintf(stderr
            , "  %s\t%s\t%s\t%s\t%s"
            "\t%s\t%s\t%s\t%s\t%s\t%s"
            "\n"
            , "blk no  "
            , "block add "
            , "next add  "
            , "prev add  "
            , "data add  "

            , "blk off  "
            , "dat off  "
            , "capacity "
            , "size     "

            , "blk size "
            , "status   "
        );
		// printf("head: %p\n", head);
    for (curr = head, i = 0; curr != NULL; curr = curr->next, i++) {
        if (leaks_only == FALSE || (leaks_only == TRUE && curr->free == FALSE)) {
            fprintf(stderr
                    , "  %u\t\t%9p\t%9p\t%9p\t%9p\t%u\t\t%u\t\t"
                      "%u\t\t%u\t\t%u\t\t%s\t%c\n"
                    , i
                    , curr
                    , curr->next
                    , curr->prev
                    , curr->data
                    , (unsigned) ((void *) curr - lower_mem_bound)
                    , (unsigned) ((void *) curr->data - lower_mem_bound)
                    , (unsigned) curr->capacity
                    , (unsigned) curr->size
                    , (unsigned) (curr->capacity + sizeof(struct block_list))
                    , curr->free ? "free  " : "in use"
                    , curr->free ? '*' : ' '
                );
            user_bytes += curr->size;
            capacity_bytes += curr->capacity;
            block_bytes += curr->capacity + sizeof(struct block_list);
            if (curr->free == FALSE && leaks_only == TRUE) {
                leak_count++;
            }
            if (curr->free == TRUE) {
                free_blocks++;
            }
            else {
                used_blocks++;
            }
        }
    }
    if (leaks_only) {
        if (leak_count == 0) {
            fprintf(stderr, "  *** No leaks found!!! That does NOT mean no leaks are possible. ***\n");
        }
        else {
            fprintf(stderr
                    , "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                      "%u\t\t%u\t\t%u\n"
                    , "Total bytes lost"
                    , capacity_bytes
                    , user_bytes
                    , block_bytes
                );
        }
    }
    else {
        fprintf(stderr
                , "  %s\t\t\t\t\t\t\t\t\t\t\t\t"
                "%u\t\t%u\t\t%u\n"
                , "Total bytes used"
                , capacity_bytes
                , user_bytes
                , block_bytes
            );
        fprintf(stderr, "  Used blocks: %u  Free blocks: %u  "
             "Min heap: %p    Max heap: %p\n"
               , used_blocks, free_blocks
               , lower_mem_bound, upper_mem_bound
            );
    }
}
