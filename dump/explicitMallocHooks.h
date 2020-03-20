/*
	Mode: C

	explicitMallocHooks.h

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef explicit_Malloc_Hooks_H
#define explicit_Malloc_Hooks_H

#include "dataStructures.h"

void* exp_malloc_hook(size_t size);
void exp_free_hook(void* ptr_address, size_t size);
void* exp_calloc_hook(size_t n_member, size_t size);
void* exp_realloc_hook(void* ptr_address, size_t size, size_t old_size);

#endif