#ifndef malloc_Wrappers_C
#define malloc_Wrappers_C

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#ifndef __MALLOC_WRAPPERS_1
#define __MALLOC_WRAPPERS_1
#endif // __MALLOC_WRAPPERS_1

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "mallocWrappers.h"
#include "../common/utilities.h"

/*
	preload constructors/destructors/helpers
*/
void __malloc_preload_initialize() {

	real_malloc = dlsym(RTLD_NEXT, "malloc");
	if(real_malloc == NULL) { puts("ERROR: pre-loading malloc dlsym "); puts(dlerror()); exit(1);}

	real_calloc = dlsym(RTLD_NEXT, "calloc");
	if(real_calloc == NULL) { puts("ERROR: pre-loading calloc dlsym "); puts(dlerror()); exit(1);}

	real_realloc = dlsym(RTLD_NEXT, "realloc");
	if(real_realloc == NULL) { puts("ERROR: pre-loading realloc dlsym "); puts(dlerror()); exit(1);}

	real_free = dlsym(RTLD_NEXT, "free");
	if(real_free == NULL) { puts("ERROR: pre-loading free dlsym "); puts(dlerror()); exit(1);}

	size_t size = __malloc_preload_table_size * sizeof(T__maloc_preload);
	__malloc_preload_table = real_malloc(size);
	init_ipc();
	shmem->requested_memory += size;
	shmem->usable_allocation += malloc_usable_size(__malloc_preload_table);
	shmem->current_requested_memory += size;
	shmem->current_usable_allocation += malloc_usable_size(__malloc_preload_table);
}

void __malloc_preload_print_stats() {

	printf("\
              malloc\
                free\
              calloc\
             realloc\
            requsted\
              usable\
             cur_req\
             cur_use\n");

	printf("%20lu%20lu%20lu%20lu%20llu%20llu%20llu%20llu\n",
		shmem->malloc_count,
		shmem->free_count,
		shmem->calloc_count,
		shmem->realloc_count,
		shmem->requested_memory,
		shmem->usable_allocation,
		shmem->current_requested_memory,
		shmem->current_usable_allocation
	);
}

void __malloc_preload_startup (void) {
	size_t size = 10;
	void* temp = malloc(size);

#ifdef MALLOC_PRELOAD_DEBUG
	printf("preload startup: %p %ld %ld\n", temp, size, malloc_usable_size(temp));
#endif
	free(temp);

#ifdef MALLOC_PRELOAD_DEBUG
	printf("preload startup ended\n");
#endif
	return;
}

void __malloc_preload_cleanup() {
	printf("\nproc ended\n");
	shmem->usable_allocation -= malloc_usable_size(__malloc_preload_table);
	shmem->current_usable_allocation -= malloc_usable_size(__malloc_preload_table);
	shmem->requested_memory -= __malloc_preload_table_size * sizeof(T__maloc_preload);
	shmem->current_requested_memory -= __malloc_preload_table_size * sizeof(T__maloc_preload);

	__malloc_preload_print_stats();
	real_free(__malloc_preload_table);
	__malloc_preload_table = NULL;
	end_ipc();
}

/*
	preloaded malloc functions
*/
void* malloc(size_t size) {

	if(__malloc_preload_init == 0) {
		__malloc_preload_init = 1;
		__malloc_preload_initialize();
		__malloc_preload_init = 2;
	} else if(__malloc_preload_init == 1) {
		void* ptr = __malloc_preload_temp_buff + __malloc_preload_temp_bumper;
		__malloc_preload_temp_bumper += size;
#ifdef MALLOC_PRELOAD_DEBUG
		if(__malloc_preload_temp_bumper >= __malloc_preload_buff_size) { puts("Increase temp buffer size in preload"); exit(1); }
		puts("preload: malloc called during initialization\n");
#endif
		return ptr;
	}

	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;

	void* ptr = NULL;
	ptr = real_malloc(size);

	shmem->malloc_count += entrance;
	if(entrance == 1) { __malloc_preload_table_insert(ptr, size); }

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;

	return ptr;
}

void free(void *ptr) {

	if ((((void*)__malloc_preload_temp_buff + 0) <= ptr) && (ptr < ((void*)__malloc_preload_temp_buff + __malloc_preload_buff_size))) {
#ifdef MALLOC_PRELOAD_DEBUG
		puts("preload: freed a pointer to temporary heap\n");
#endif
		return;
	}

	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;

	shmem->free_count += entrance;
	if(entrance == 1) { int table_status = __malloc_preload_table_invalidate(ptr); }

	real_free(ptr);

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return;
}

void *calloc(size_t nmemb, size_t size) {

	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;

	void* ptr = NULL;
	ptr = real_calloc(nmemb, size);

	shmem->calloc_count += entrance;
	if(entrance == 1) { __malloc_preload_table_insert(ptr, nmemb * size); }

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return ptr;
}

void *realloc(void *ptr, size_t size) {

	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;

	void* new_ptr = NULL;
	new_ptr = real_realloc(ptr, size);

	shmem->realloc_count += entrance;
	if(entrance == 1) { int status = __malloc_preload_table_invalidate(ptr); __malloc_preload_table_insert(new_ptr, size); }

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return new_ptr;
}


/*
	dynamic memory records
*/
void __malloc_preload_table_insert(void* ptr, size_t size) {
	if(ptr == NULL) { return; }

	if (__malloc_preload_table_size <= __malloc_preload_table_bumper) {
#ifdef MALLOC_PRELOAD_DEBUG
		printf("preload: increasing table size %p\n", __malloc_preload_table);
#endif
		shmem->usable_allocation -= malloc_usable_size(__malloc_preload_table);
		shmem->current_usable_allocation -= malloc_usable_size(__malloc_preload_table);

		__malloc_preload_table_size += 1000;
		__malloc_preload_table = real_realloc(__malloc_preload_table, __malloc_preload_table_size * sizeof(T__maloc_preload));

		shmem->requested_memory += 1000 * sizeof(T__maloc_preload);
		shmem->usable_allocation += malloc_usable_size(__malloc_preload_table);
		shmem->current_requested_memory += 1000 * sizeof(T__maloc_preload);
		shmem->current_usable_allocation += malloc_usable_size(__malloc_preload_table);
#ifdef MALLOC_PRELOAD_DEBUG
		printf("preload: increased table size %p\n", __malloc_preload_table);
#endif
	}

	__malloc_preload_table[__malloc_preload_table_bumper] = (T__maloc_preload){ptr, size};
	__malloc_preload_table_bumper += 1;


	size_t usable_size = malloc_usable_size(ptr);
	shmem->requested_memory += size;
	shmem->usable_allocation += usable_size;
	shmem->current_requested_memory += size;
	shmem->current_usable_allocation += usable_size;

	return;
}

int __malloc_preload_table_invalidate(void* ptr) {
	if(ptr == NULL) { return 1; }

	for (int i = __malloc_preload_table_bumper-1; i >= 0; --i) {
		if(__malloc_preload_table[i].ptr == ptr) {

#ifdef MALLOC_PRELOAD_DEBUG
			if(__malloc_preload_table[i].size == -1) { printf("preload: double free in invalidate %p\n", ptr); return -1; }
#endif
			shmem->current_usable_allocation -= malloc_usable_size(ptr);
			shmem->current_requested_memory -= __malloc_preload_table[i].size;
			__malloc_preload_table[i].size = -1;

			return 1;
		}
	}

#ifdef MALLOC_PRELOAD_DEBUG
	printf("preload: pointer %p not found in table: %p\n", ptr, __malloc_preload_table);
#endif

	return 0;
}

#endif // malloc_Wrappers_C