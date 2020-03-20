/*
	Mode: C

	explicitMallocStats.c

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef explicit_Malloc_Stats_C
#define explicit_Malloc_Stats_C

#include "explicitMallocStats.h"

memory_snapshot parse_proc_maps(char* fname) {
	memory_snapshot mem_snapshot = {0};
	int in_mmap = 0;

	char buffer[1000];
	FILE* fptr = fopen(fname, "r");

	while(fgets(buffer, 1000, fptr) != NULL) {
		char* addr_space_start = (char*) strtok(buffer, "-");
		char* addr_space_end = strtok(NULL, " ");
		unsigned long int bytes = (unsigned long int) (strtol(addr_space_end, NULL, 16) - strtol(addr_space_start, NULL, 16));

		// ignore next 4 coulumns in line
		char* token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");

		char* owner = strtok(NULL, " ");

		if ((owner[0] == '/') && (in_mmap == 0)) {
			mem_snapshot.text += bytes;
		} else if (strcmp(owner, "[heap]\n") == 0) {
			mem_snapshot.heap += bytes;
			in_mmap = 1;
		} else if ((owner[0] == '/') && (in_mmap == 1)) {
			mem_snapshot.mmap_so += bytes;
		} else if ((strcmp(owner, "\n") == 0) && (in_mmap == 1)) {
			mem_snapshot.mmap += bytes;
		} else if (strcmp(owner, "[stack]\n") == 0) {
			mem_snapshot.stack += bytes;
			in_mmap = 0;
		} else if (strcmp(owner, "[vvar]\n") == 0) {
			mem_snapshot.vvar += bytes;
		} else if (strcmp(owner, "[vdso]\n") == 0) {
			mem_snapshot.vdso += bytes;
		} else if (strcmp(owner, "[vsyscall]\n") == 0) {
			mem_snapshot.vsyscall += bytes;
		} else {
			mem_snapshot.unfigured += bytes;
		}
	}

	FUNC_PARSE_PROC_MAPS_END:
	fclose(fptr); fptr = NULL;
	return mem_snapshot;
}

/*fragmentation calculate_fragmentation() {
	fragmentation cur_frag = {0};

	long int bench_in_use = exp_malloc_stats.cur_in_use_bytes - exp_malloc_stats.pre_in_use_bytes;
	long int bench_heap = exp_malloc_stats.cur_proc_maps.heap; // - exp_malloc_stats.pre_proc_maps.heap;
	long int bench_mmap = exp_malloc_stats.cur_proc_maps.mmap - exp_malloc_stats.pre_proc_maps.mmap;

	cur_frag.ref_bytes = exp_malloc_stats.current_requested_memory;
	cur_frag.alignment = exp_malloc_stats.current_usable_allocation - exp_malloc_stats.current_requested_memory;
//	cur_frag.bookkeeping = bench_in_use - exp_malloc_stats.current_usable_allocation;
	cur_frag.internal = cur_frag.alignment;

//	if (cur_frag.bookkeeping >= 0) {
//		cur_frag.internal = cur_frag.alignment + cur_frag.bookkeeping;
//	}

	cur_frag.external = (bench_mmap + bench_heap) - (exp_malloc_stats.current_usable_allocation);
	printf("%ld %ld %ld %ld\n", bench_in_use, bench_heap, bench_mmap, cur_frag.external);

	FUNC_CALCULATE_FRAGMENTATION_END:
	return cur_frag;
}*/

///////////////////////////////////////////////////////
//         Print Functions For Data Structures       //
///////////////////////////////////////////////////////

/*
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
"
, exp_malloc_stats.int_malloc_stats.malloc_count, exp_malloc_stats.int_malloc_stats.free_count, exp_malloc_stats.int_malloc_stats.calloc_count
, exp_malloc_stats.int_malloc_stats.realloc_count, exp_malloc_stats.int_malloc_stats.requested_memory, exp_malloc_stats.int_malloc_stats.usable_allocation
, exp_malloc_stats.int_malloc_stats.current_requested_memory, exp_malloc_stats.int_malloc_stats.current_usable_allocation);

	printf("	...\n\
	init proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.init_mem_maps);

	printf("	...\n\
	pre proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.pre_mem_maps);

	printf("	...\n\
	cur proc maps:\n");
	print_memory_snapshot(exp_malloc_stats.cur_mem_maps);
}

void print_fragmentation(fragmentation frag) {
	printf("Total memory needed:		%lld\n\
Fragmentation:\n\
	internal:			%lld %Lf%%\n\
	external:			%lld %Lf%%\n\
	total:				%lld %Lf%%\n"
	, frag.ref_bytes, frag.internal, frag.internal * (long double) 100.0 / frag.ref_bytes, frag.external, frag.external * (long double) 100.0 / frag.ref_bytes
	, frag.internal + frag.external, (frag.internal + frag.external) * (long double) 100.0 / frag.ref_bytes);

	if(frag.bookkeeping >= 0) {
	printf("\
Internal fragmentation details:\n\
	alignment:	%lld %Lf%%\n\
	bookkeeping:	%lld %Lf%%\n"
	, frag.alignment, frag.alignment * (long double) 100.0 / frag.ref_bytes, frag.bookkeeping, frag.bookkeeping * (long double) 100.0 / frag.ref_bytes);
	} else {
	printf("\
Internal fragmentation details:\n\
	alignment:	%lld %Lf%%\n"
	, frag.alignment, frag.alignment * (long double) 100.0 / frag.ref_bytes);
		printf("*FRAGMENTATION CAUSED BY MALLOC BOOKKEEPING IS PART OF EXTERNAL FRAGMENTATION*\n");
	}

}
*/
#endif