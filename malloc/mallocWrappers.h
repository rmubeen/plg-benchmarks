#ifndef malloc_Wrappers_H
#define malloc_Wrappers_H

#include "stdint.h"

#include "../common/dataStructures.h"

/*
	preload constructors/destructors/helpers
*/
int __malloc_preload_func_called = 0;
void __malloc_preload_initialize();
void __malloc_preload_print_stats();
void __malloc_preload_startup (void) __attribute__ ((constructor));
void __malloc_preload_cleanup (void) __attribute__ ((destructor));

/*
	real malloc functions
*/
static void* (*real_malloc)(size_t) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;
static void* (*real_realloc)(void*, size_t) = NULL;
static void (*real_free)(void*) = NULL;
#ifdef __MALLOC_WRAPPERS_2
static size_t (*real_malloc_usable_size)(void*);
#endif
/*
	temporary malloc heap
*/
size_t __malloc_preload_init = 0;
size_t __malloc_preload_temp_bumper = 0;
size_t __malloc_preload_buff_size = 8192; char __malloc_preload_temp_buff[8192]; // If changing this number then also change it inside free function definition

/*
	preloaded malloc functions
*/
void* malloc(size_t size);
void free(void *ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void *ptr, size_t size);
#ifdef __MALLOC_WRAPPERS_2
size_t malloc_usable_size(void *ptr);
#endif
/*
	dynamic memory records
*/
#ifdef __MALLOC_WRAPPERS_1
T__maloc_preload* __malloc_preload_table;
unsigned long long __malloc_preload_table_size = 1000;
unsigned long long __malloc_preload_table_bumper = 0;
void __malloc_preload_table_insert(void* ptr, size_t size);
int __malloc_preload_table_invalidate(void* ptr);
#elif defined(__MALLOC_WRAPPERS_2)
const uintptr_t __malloc_preload_head_size = 2 * ((uintptr_t)sizeof(uintptr_t));
void* __malloc_preload_register_allocation(void* real_ptr, size_t bookkeeping_size, size_t size);
void* __malloc_preload_unregister_allocation(void* ptr);
#endif // __MALLOC_WRAPPERS_1


#endif // malloc_Wrappers_H