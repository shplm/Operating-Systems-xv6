#include "beavalloc.h"
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

/************************************************************
 * 目的：模拟标准库函数malloc和free，实现进程堆上的内存管理。
 * 实现细节：1.使用显示双向链表，按地址顺序维护内存块。
 *           2.使用循环首次适应算法进行空闲块的选择。
 ***********************************************************/
 #define has_used 1
 #define has_not_used 0
 // #define MINSIZE 32
 // #define SIZE(p) (p->size)
 // #define USED(p) (p->free)
 // #define PRED(p) (p->prev)
 // #define SUCC(p) (p->next)

 void malloc_init();
 // void* beavalloc(size_t size);
 // void beavfree(void *p);

/*内存块数据结构*/
struct mem_block{
	size_t size;
	size_t capacity;
	struct mem_block* prev;
	struct mem_block* next;
	int free;
	// int in_used;
	void *data;
};

// static inline size_t word_align(size_t size){
// 	return size + (sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
// }


/**********************************
 * 全局变量定义：
 * 1.内存块头指针
 * 2.内存块尾指针
 * 3.分配器初始化标记
 * 4.遍历内存块的标记
 *********************************/
 struct mem_block* head;
 struct mem_block* tail;
 struct mem_block* flag;
 int has_initialized = 0;


void *lower_mem_bound = 0;
void *upper_mem_bound = 0;

// void *first_base = sbrk(0);

void beavalloc_reset(void){
	has_initialized = 0;
	brk(head);
	// first_base = NULL;
	//brk(managed_memory_start);//reset to the initial heap locaiton
}



void malloc_init(){
	head = sbrk(sizeof(struct mem_block));
	tail = sbrk(sizeof(struct mem_block));
	head->free = tail->free = 1;
	head->prev = tail;
	tail->prev = head;
	head->next = tail;
	tail->next = head;
	flag = head;
	has_initialized = 1;
  // lower_mem_bound = first_base;
  // last_valid_address = sbrk(0);
}


void* beavalloc(size_t size){

	struct mem_block *cur_block;
	struct mem_block *pred_block;
	void *mem_location = NULL;
	long block_size = size + sizeof(struct mem_block);
	struct mem_block *alloc_block = NULL;

	if(size <= 0)
		return NULL;

	if(!has_initialized){
		malloc_init();
	}

	cur_block = flag;
	while(cur_block){
    		if(!cur_block->free){
        			if(cur_block->size >= size){       /*将cur_block标记为已分配*/
        				cur_block->free = 1;
        				mem_location = (void*) cur_block + sizeof(struct mem_block);
            				if(cur_block->size - size > sizeof(struct mem_block)){          /*判断是否需要分割当前块*/
            					pred_block = cur_block->next;
            					alloc_block = (void*) cur_block + block_size;
            					alloc_block->free = 0;
            					alloc_block->size = cur_block->size - size - sizeof(struct mem_block);
            					alloc_block->prev = cur_block;
            					alloc_block->next = pred_block;
            					cur_block->next = alloc_block;
            					cur_block->size = size;
            					pred_block->prev = alloc_block;
            				  }
        				break;
    			       }
		      }
		cur_block=cur_block->next;
		if(cur_block == flag)
			 break;
	}//end while
	flag = cur_block;


	/*没有找到合适的块，申请新的空间*/
	/*这一步很关键，必须注意将tail块放置到最后才能保证链表中块的地址顺序*/
	if(!mem_location){
		pred_block = tail->prev;

		sbrk(-1*sizeof(struct mem_block));
		if( (long) (alloc_block = sbrk(block_size)) == -1){
			printf("Failed to allocate memory!\n");
			return NULL;
		}
		tail = sbrk(sizeof(struct mem_block));

		alloc_block->size = size;
		alloc_block->free = has_used;
		alloc_block->next = tail;
		alloc_block->prev = pred_block;
		pred_block->next = alloc_block;

		tail->prev = alloc_block;
		tail->next = head;
		head->free = has_used;
		tail->size = 0;

		head->prev = tail;

		mem_location = (void*) alloc_block + sizeof(struct mem_block);
	}
  upper_mem_bound = mem_location;
  // void* size_of_struct = sizeof(struct mem_block);
  // mem_location = mem_location + size_of_struct;
	return mem_location;
}


void beavfree(void *p){
	if(!p)
		return;

    struct mem_block* cur_block = (void*)p - sizeof(struct mem_block);
    struct mem_block* pred_block;
    struct mem_block* succ_block;

    pred_block = cur_block->prev;
    succ_block = cur_block->next;

	// struct mem_block* cur_block = (void *)p - sizeof(struct mem_block);
	// /* 判断是否合并，四种情况，1.不合并。2.前合并。3.后合并。4.前后同时合并 */
	// /* 边界上的问题已经通过初始化解决了*/
  //
	// struct mem_block* pred_block = cur_block->prev;
	// struct mem_block* succ_block = cur_block->next;

	if(pred_block->free && succ_block->free){
		cur_block->free = has_not_used;
	}
	else if(!(pred_block->free) && succ_block->free){
		pred_block->size = pred_block->size + cur_block->size + sizeof(struct mem_block);
		pred_block->next = cur_block->next;
		succ_block->prev = pred_block;
		flag = pred_block;
	}

	else if(pred_block->free && !(succ_block->free)){
		cur_block->free = has_not_used;
		cur_block->size = cur_block->size + succ_block->size + sizeof(struct mem_block);
		cur_block->next = succ_block->next;
		succ_block->next->prev = cur_block;
	  flag = cur_block;
	}

	else if( !(pred_block->free) && !(succ_block->free)){
		pred_block->size = pred_block->size + cur_block->size + succ_block->size + 2 * sizeof(struct mem_block);
		pred_block->next = succ_block->next;
		succ_block->next->prev = pred_block;
		flag = pred_block;
	}
}


void memory_state(){
	struct mem_block* cur_block = head;
	while(cur_block){
		printf("size: %ld, used: %d,address: %p\n",cur_block->size,cur_block->free,cur_block);
		cur_block = cur_block->next;
		if(cur_block == head)
			break;
	}
	printf("\n");
}



void beavalloc_set_verbose(uint8_t flag){

}


void *beavcalloc(size_t nmemb, size_t size){
	if(nmemb == 0 || size == 0){
		return NULL;
	}

}



void *beavrealloc(void *ptr, size_t size){
	if(size <= 0){
		return NULL;
	}
	//allocate 2 times of size when ptr is null
	if(ptr == NULL){
		return beavalloc(2*size);
	}


}




void beavalloc_dump(uint leaks_only)
{
    struct mem_block *curr = NULL;
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

    for (curr = head, i = 0; curr != tail; curr = curr->next, i++) {
        if (leaks_only == FALSE || (leaks_only == TRUE && curr->free == 1)) {
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
                    , (unsigned) (curr->capacity + sizeof(struct mem_block))
                    , curr->free ? "in use  " : "free"
                    , curr->free ? ' ' : '*'
                );
            user_bytes += curr->size;
            capacity_bytes += curr->capacity;
            block_bytes += curr->capacity + sizeof(struct mem_block);
            if (curr->free == 1 && leaks_only == TRUE) {
                leak_count++;
            }
            if (curr->free == 0) {
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
