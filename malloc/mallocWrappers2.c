#ifndef malloc_Wrappers_C
#define malloc_Wrappers_C

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#ifndef __MALLOC_WRAPPERS_2
#define __MALLOC_WRAPPERS_2
#endif // __MALLOC_WRAPPERS_2

#include <math.h>
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

	real_malloc_usable_size = dlsym(RTLD_NEXT, "malloc_usable_size");
	if(real_malloc_usable_size == NULL) { puts("ERROR: pre-loading malloc_usable_size dlsym "); puts(dlerror()); exit(1);}

	init_ipc();
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
	printf("preload startup: %p %ld %ld %ld\n", temp, size, malloc_usable_size(temp), real_malloc_usable_size(temp));
#endif
	free(temp);

#ifdef MALLOC_PRELOAD_DEBUG
	printf("preload startup ended\n");
#endif
	return;
}

void __malloc_preload_cleanup() {
	printf("\nproc ended\n");
	__malloc_preload_print_stats();
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
	size_t bookkeeping_size = 0;

	if(entrance == 1) { bookkeeping_size = __malloc_preload_head_size; }
	void* ptr = NULL;
	ptr = real_malloc(size + bookkeeping_size);

	if(entrance == 1) {
		shmem->malloc_count += 1;
		ptr = __malloc_preload_register_allocation(ptr, bookkeeping_size, size);
	}

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return ptr;
}

void free(void *ptr) {
	if ((((void*)__malloc_preload_temp_buff + 0) <= ptr) && (ptr < ((void*)__malloc_preload_temp_buff + __malloc_preload_buff_size))) { return;	}
	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;


	if(entrance == 1) {
		shmem->free_count += 1;
		ptr = __malloc_preload_unregister_allocation(ptr);
	}
	real_free(ptr);

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return;
}

void *calloc(size_t nmemb, size_t size) {
	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;
	size_t bookkeeping_size = 0;

	if(entrance == 1) { bookkeeping_size = ceil(((double)__malloc_preload_head_size) / size); }
	void* ptr = NULL;
	ptr = real_calloc(nmemb + bookkeeping_size, size);

	if(entrance == 1) {
		shmem->calloc_count += 1;
		ptr = __malloc_preload_register_allocation(ptr, bookkeeping_size*size, nmemb*size);
	}

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return ptr;
}

void *realloc(void *ptr, size_t size) {
	int entrance = __malloc_preload_func_called == 0 ? 1 : 0;
	__malloc_preload_func_called = 1;
	size_t bookkeeping_size = 0;

	if(entrance == 1) {
		bookkeeping_size = __malloc_preload_head_size;
		ptr = __malloc_preload_unregister_allocation(ptr);
	}
	void* new_ptr = NULL;
	new_ptr = real_realloc(ptr, size + bookkeeping_size);

	if(entrance == 1) {
		shmem->realloc_count += 1;
		new_ptr = __malloc_preload_register_allocation(new_ptr, bookkeeping_size, size);
	}

	__malloc_preload_func_called = entrance == 1 ? 0 : 1;
	return new_ptr;
}

size_t malloc_usable_size(void *ptr) {
	uintptr_t* real_ptr = (uintptr_t*)(((uintptr_t)ptr) - __malloc_preload_head_size);
	real_ptr = (uintptr_t*)(real_ptr[0]);

	uintptr_t bookkeeping_size = (uintptr_t)ptr - (uintptr_t)real_ptr;
	uintptr_t usable_size = ((uintptr_t)real_malloc_usable_size(real_ptr)) - bookkeeping_size;

#ifdef MALLOC_PRELOAD_DEBUG
	printf("usable\t\t real: %p, ptr: %p\n", real_ptr, ptr);
#endif
	return (size_t)usable_size;
}

/*
	dynamic memory records
*/
void* __malloc_preload_register_allocation(void* real_ptr, size_t bookkeeping_size, size_t size) {
	if(__malloc_preload_head_size > bookkeeping_size) { puts("preload: malloc preload head size > bookkeeping space available\n"); exit(1); }
	uintptr_t* ptr = (uintptr_t*)(((uintptr_t)real_ptr) + ((uintptr_t)bookkeeping_size) - __malloc_preload_head_size);

	ptr[0] = (uintptr_t)real_ptr;
	ptr[1] = (uintptr_t)size;
	ptr = (uintptr_t*)(((uintptr_t)ptr) + __malloc_preload_head_size);

	uintptr_t requested_size = bookkeeping_size + size;
	uintptr_t usable_size = real_malloc_usable_size(real_ptr);
	shmem->requested_memory += requested_size;
	shmem->usable_allocation += usable_size;
	shmem->current_requested_memory += requested_size;
	shmem->current_usable_allocation += usable_size;

#ifdef MALLOC_PRELOAD_DEBUG
	printf("register\t real: %p, ptr: %p\n", real_ptr, ptr);
#endif
	return (void*)ptr;
}

void* __malloc_preload_unregister_allocation(void* ptr) {
	uintptr_t* real_ptr = (uintptr_t*)(((uintptr_t)ptr) - __malloc_preload_head_size);

	uintptr_t size = real_ptr[1];
	real_ptr = (uintptr_t*)(real_ptr[0]);
	uintptr_t bookkeeping_size = (uintptr_t)ptr - (uintptr_t)real_ptr;

	uintptr_t requested_size = bookkeeping_size + size;
	uintptr_t usable_size = real_malloc_usable_size(real_ptr);
	shmem->current_usable_allocation -= usable_size;
	shmem->current_requested_memory -= requested_size;

#ifdef MALLOC_PRELOAD_DEBUG
	printf("unregister\t real: %p, ptr: %p\n", real_ptr, ptr);
#endif
	if(__malloc_preload_head_size > bookkeeping_size) { puts("preload: malloc preload head size > bookkeeping space calculated\n"); exit(1); }
	return (void*)real_ptr;
}

#endif // malloc_Wrappers_C