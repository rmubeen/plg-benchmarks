/*
	Mode: C

	explicitMallocHooks.c

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef explicit_Malloc_Hooks_C
#define explicit_Malloc_Hooks_C

#include "stdio.h"
#include "string.h"
#include "malloc.h"

#include "explicitMallocHooks.h"

void* exp_malloc_hook(size_t size) {
	void* ptr_address = malloc(size);
	int_malloc_stats->malloc_count += 1;

	int_malloc_stats->requested_memory += size;
	int_malloc_stats->usable_allocation += malloc_usable_size(ptr_address);
	int_malloc_stats->current_requested_memory += size;
	int_malloc_stats->current_usable_allocation += malloc_usable_size(ptr_address);

	return ptr_address;
}

void exp_free_hook(void* ptr_address, size_t size) {
	int_malloc_stats->current_requested_memory -= size;
	int_malloc_stats->current_usable_allocation -= malloc_usable_size(ptr_address);

	free(ptr_address);
	int_malloc_stats->free_count += 1;

	return;
}

void* exp_calloc_hook(size_t n_member, size_t size) {
	void* ptr_address = calloc(n_member, size);
	int_malloc_stats->calloc_count += 1;

	int_malloc_stats->requested_memory += size;
	int_malloc_stats->usable_allocation += malloc_usable_size(ptr_address);
	int_malloc_stats->current_requested_memory += size;
	int_malloc_stats->current_usable_allocation += malloc_usable_size(ptr_address);

	return ptr_address;
}

void* exp_realloc_hook(void* ptr_address, size_t size, size_t old_size) {
	size_t old_usable_size = malloc_usable_size(ptr_address);
	int size_diff = size - old_size;

	void* new_ptr_address = realloc(ptr_address, size);
	int_malloc_stats->realloc_count += 1;


	int_malloc_stats->requested_memory += size;
	int_malloc_stats->usable_allocation += malloc_usable_size(new_ptr_address);
	int_malloc_stats->current_requested_memory += size_diff;
	int_malloc_stats->current_usable_allocation += malloc_usable_size(new_ptr_address) - old_usable_size;

	return new_ptr_address;
}

#endif