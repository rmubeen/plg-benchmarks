/*
	Mode: C

	dataStructures.h

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef data_Structures_H
#define data_Structures_H

#include <stdlib.h>

struct s_parameters {
	size_t min_obj_size;
	size_t max_obj_size;
	size_t obj_size_step;
	char* size_distro;

	float min_obj_life;		// micro-seconds
	float max_obj_life;		// micro-seconds
	float obj_life_step;
	char* life_distro;

	long int num_objects;
	int num_threads;

	int verbose_flag;		// only 0 means false
} knobs;

struct s_int_malloc_statistics {
	unsigned long int malloc_count;
	unsigned long int free_count;
	unsigned long int calloc_count;
	unsigned long int realloc_count;

	unsigned long long requested_memory;
	unsigned long long usable_allocation;
	unsigned long long current_requested_memory;
	unsigned long long current_usable_allocation;

	unsigned long int init_sys_bytes;
	unsigned long int init_in_use_bytes;
	unsigned long int init_mmap_bytes;

	unsigned long int pre_sys_bytes;
	unsigned long int pre_in_use_bytes;
	unsigned long int pre_mmap_bytes;

	unsigned long int cur_sys_bytes;
	unsigned long int cur_in_use_bytes;
	unsigned long int cur_mmap_bytes;

};
struct s_int_malloc_statistics int_malloc_stats = {0};

typedef struct s_pointers_data {
	void* ptr;
	size_t requested_memory;
} td_pointers_data;
td_pointers_data* pointers_data;

typedef struct s_memory_snapshot {
	unsigned long int text;

	unsigned long int heap;
	unsigned long int mmap_so;
	unsigned long int mmap;

	unsigned long int stack;
	unsigned long int vvar;
	unsigned long int vdso;
	unsigned long int vsyscall;

	unsigned long int unfigured;
} memory_snapshot;

void print_memory_snapshot(memory_snapshot ms) {
	printf("\
	text:			%lu\n\
	heap:			%lu\n\
	mmap_so:		%lu\n\
	mmap:			%lu\n\
	stack:			%lu\n\
	vvar:			%lu\n\
	vdso:			%lu\n\
	vsyscall:		%lu\n\
	unfigured:		%lu\n"
	, ms.text, ms.heap, ms.mmap_so, ms.mmap, ms.stack, ms.vvar, ms.vdso, ms.vsyscall, ms.unfigured);
}

struct s_exp_malloc_statistics {

	struct s_int_malloc_statistics int_malloc_stats;

	memory_snapshot init_proc_maps;
	memory_snapshot pre_proc_maps;
	memory_snapshot cur_proc_maps;
} exp_malloc_stats;

void print_exp_malloc_stats() {
	printf("Explicit Malloc Stats:\n\
	malloc count: 	%lu\n\
	free count: 	%lu\n\
	calloc count: 	%lu\n\
	realloc count: 	%lu\n\
	...\n\
	requested memory: 	%llu\n\
	usable allocation: 	%llu\n\
	cur requested memory: 	%llu\n\
	cur usable alloc: 	%llu\n\
	...\n\
	*STATS IN THIS SECTION ARE SUBJECT TO COORRECTNESS OF FUNCTION \"struct mallinfo mallinfo(void)\" IN YOUR MALLOC.*\n\
	*See \"man mallinfo\" for more info.*\n\
	init system bytes: 	%lu\n\
	init in-use bytes: 	%lu\n\
	init mmap bytes: 	%lu\n\
	pre system bytes: 	%lu\n\
	pre in-use bytes: 	%lu\n\
	pre mmap bytes: 	%lu\n\
	cur system bytes: 	%lu\n\
	cur in-use bytes: 	%lu\n\
	cur mmap bytes: 	%lu\n\
"
, exp_malloc_stats.malloc_count, exp_malloc_stats.free_count, exp_malloc_stats.calloc_count, exp_malloc_stats.realloc_count, exp_malloc_stats.requested_memory
, exp_malloc_stats.usable_allocation, exp_malloc_stats.current_requested_memory, exp_malloc_stats.current_usable_allocation, exp_malloc_stats.init_sys_bytes
, exp_malloc_stats.init_in_use_bytes, exp_malloc_stats.init_mmap_bytes, exp_malloc_stats.pre_sys_bytes, exp_malloc_stats.pre_in_use_bytes
, exp_malloc_stats.pre_mmap_bytes, exp_malloc_stats.cur_sys_bytes, exp_malloc_stats.cur_in_use_bytes, exp_malloc_stats.cur_mmap_bytes);

	printf("	...\n\
	init proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.init_proc_maps);

	printf("	...\n\
	pre proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.pre_proc_maps);

	printf("	...\n\
	cur proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.cur_proc_maps);
}

typedef struct s_fragmentation {
	long long int alignment;
	long long int bookkeeping;
	long long int internal; // alignment + bookkeeping
	long long int external;

	long long int ref_bytes;
} fragmentation;

void print_fragmentation(fragmentation frag) {
	printf("\
Total memory needed:		%lld\n\
Fragmentation:\n\
	internal:			%lld (%Lf%)\n\
	external:			%lld (%Lf%)\n\
	total:				%lld (%Lf%)\n"
	, frag.ref_bytes, frag.internal, frag.internal * (long double) 100.0 / frag.ref_bytes, frag.external, frag.external * (long double) 100.0 / frag.ref_bytes
	, frag.internal + frag.external, (frag.internal + frag.external) * (long double) 100.0 / frag.ref_bytes);

	if(frag.bookkeeping >= 0) {
	printf("\
Internal fragmentation details:\n\
	alignment:	%lld (%Lf%)\n\
	bookkeeping:	%lld (%Lf%)\n"
	, frag.alignment, frag.alignment * (long double) 100.0 / frag.ref_bytes, frag.bookkeeping, frag.bookkeeping * (long double) 100.0 / frag.ref_bytes);
	} else {
	printf("\
Internal fragmentation details:\n\
	alignment:	%lld (%Lf%)\n"
	, frag.alignment, frag.alignment * (long double) 100.0 / frag.ref_bytes);
		printf("*FRAGMENTATION CAUSED BY MALLOC BOOKKEEPING IS PART OF EXTERNAL FRAGMENTATION*\n");
	}

}

#endif