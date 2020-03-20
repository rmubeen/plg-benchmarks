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

#include "stdio.h"
#include "string.h"

#include "explicitMallocStats.h"

void init_exp_malloc_stats() {
	struct mallinfo mi = mallinfo();
	exp_malloc_stats.init_sys_bytes = mi.arena;
	exp_malloc_stats.init_in_use_bytes = mi.uordblks;
	exp_malloc_stats.init_mmap_bytes = mi.hblkhd;

	exp_malloc_stats.init_proc_maps = parse_proc_maps(proc_maps_fname);
}

void start_exp_malloc_stats() {
	struct mallinfo mi = mallinfo();
	exp_malloc_stats.pre_sys_bytes = mi.arena;
	exp_malloc_stats.pre_in_use_bytes = mi.uordblks;
	exp_malloc_stats.pre_mmap_bytes = mi.hblkhd;

	exp_malloc_stats.pre_proc_maps = parse_proc_maps(proc_maps_fname);
}

void update_exp_malloc_stats() {
	struct mallinfo mi = mallinfo();
	exp_malloc_stats.cur_sys_bytes = mi.arena;
	exp_malloc_stats.cur_in_use_bytes = mi.uordblks;
	exp_malloc_stats.cur_mmap_bytes = mi.hblkhd;

	exp_malloc_stats.cur_proc_maps = parse_proc_maps(proc_maps_fname);
}

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

fragmentation calculate_fragmentation() {
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
}

#endif