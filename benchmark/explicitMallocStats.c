#ifndef explicit_Malloc_Stats_C
#define explicit_Malloc_Stats_C

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "explicitMallocStats.h"

T_memory_snapshot parse_proc_maps(char* fname) {
	T_memory_snapshot mem_snapshot = {0};
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
    mem_snapshot.total_dynamic = mem_snapshot.heap + mem_snapshot.mmap;
	fclose(fptr); fptr = NULL;
	return mem_snapshot;
}

T_memory_snapshot incr_avg_mem_shot(T_memory_snapshot avg, int n, T_memory_snapshot new_mem) {
	T_memory_snapshot new_avg;

	new_avg.text = (avg.text * n + new_mem.text) / (n + 1);

	new_avg.heap = (avg.heap * n + new_mem.heap) / (n + 1);
	new_avg.mmap = (avg.mmap * n + new_mem.mmap) / (n + 1);
	new_avg.mmap_so = (avg.mmap_so * n + new_mem.mmap_so) / (n + 1);

	new_avg.vvar = (avg.vvar * n + new_mem.vvar) / (n + 1);
	new_avg.vdso = (avg.vdso * n + new_mem.vdso) / (n + 1);
	new_avg.stack = (avg.stack * n + new_mem.stack) / (n + 1);
	new_avg.vsyscall = (avg.vsyscall * n + new_mem.vsyscall) / (n + 1);

	new_avg.unfigured = (avg.unfigured * n + new_mem.unfigured) / (n + 1);
    new_avg.total_dynamic = (avg.total_dynamic * n + new_mem.total_dynamic) / (n + 1);

	new_avg.int_malloc_stats = incr_avg_int_mem_stats(avg.int_malloc_stats, n, new_mem.int_malloc_stats);
    new_avg.fragmentation = incr_avg_fragmentation(avg.fragmentation, n, new_mem.fragmentation);

	return new_avg;
}

T_int_malloc_stats incr_avg_int_mem_stats(T_int_malloc_stats avg, int n, T_int_malloc_stats new_stats) {
	T_int_malloc_stats new_avg;

	new_avg.MSG[0] = '\0';
	new_avg.ppid = avg.ppid;
	new_avg.pid = avg.pid;

	new_avg.free_count = (avg.free_count * n + new_stats.free_count) / (n + 1);
	new_avg.malloc_count = (avg.malloc_count * n + new_stats.malloc_count) / (n + 1);
	new_avg.calloc_count = (avg.calloc_count * n + new_stats.calloc_count) / (n + 1);
	new_avg.realloc_count = (avg.realloc_count * n + new_stats.realloc_count) / (n + 1);

	new_avg.requested_memory = (avg.requested_memory * n + new_stats.requested_memory) / (n + 1);
	new_avg.usable_allocation = (avg.usable_allocation * n + new_stats.usable_allocation) / (n + 1);
	new_avg.current_requested_memory = (avg.current_requested_memory * n + new_stats.current_requested_memory) / (n + 1);
	new_avg.current_usable_allocation = (avg.current_usable_allocation * n + new_stats.current_usable_allocation) / (n + 1);

	return new_avg;
}

T_fragmentation incr_avg_fragmentation(T_fragmentation avg, int n, T_fragmentation new_frag) {
    T_fragmentation new_avg;

    new_avg.alignment = (avg.alignment * n + new_frag.alignment) / (n + 1);
    new_avg.bookkeeping = (avg.bookkeeping * n + new_frag.bookkeeping) / (n + 1);

    new_avg.internal = (avg.internal * n + new_frag.internal) / (n + 1);
    new_avg.external = (avg.external * n + new_frag.external) / (n + 1);

    new_avg.ref_bytes = (avg.ref_bytes * n + new_frag.ref_bytes) / (n + 1);
    new_avg.total = (avg.total * n + new_frag.total) / (n + 1);

    return new_avg;
}

T_fragmentation calculate_fragmentation(T_memory_snapshot mem_shot) {
    T_fragmentation fragmentation = {0};

	unsigned long int mem_alloc = mem_shot.mmap + mem_shot.heap;
	unsigned long long mem_in_use = mem_shot.int_malloc_stats.current_usable_allocation;
    unsigned long long mem_in_req = mem_shot.int_malloc_stats.current_requested_memory;
    if(mem_in_use > mem_alloc) { return fragmentation; }

    fragmentation.ref_bytes = (mem_in_req == 0) ? 1 : mem_in_req;

	fragmentation.bookkeeping = 0.0; // Not working yet
	fragmentation.alignment = ( mem_in_use - mem_in_req) * 1.0 / fragmentation.ref_bytes * 100.0;

	fragmentation.internal = fragmentation.alignment + fragmentation.bookkeeping;
	fragmentation.external = ( mem_alloc - mem_in_use) * 1.0 / fragmentation.ref_bytes * 100.0;

	fragmentation.total = fragmentation.internal + fragmentation.external;

    return fragmentation;
}

///////////////////////////////////////////////////////
//         Print Functions For Data Structures       //
///////////////////////////////////////////////////////

void print_mem_stats_layout(FILE *fp) {
    char* str_buffer = malloc(1000 * sizeof(char));
    sprintf(str_buffer, "\n\
             req_mem\
         cur_req_mem\
          in_use_mem\
      cur_in_use_mem\
                heap\
                mmap\
               total\
           int_frag%%\
           ext_frag%%\
              total%%\n"
        );

    if(fp == NULL) {printf("%s", str_buffer);}
    else {fprintf(fp, "%s", str_buffer);}
    free(str_buffer);
}

void print_mem_stats(T_memory_snapshot mem_shot, FILE *fp) {
    char* str_buffer = malloc(1000 * sizeof(char));
    sprintf(str_buffer, "%20llu %20llu %20llu %20llu %20ld %20ld %20ld %17.2Lf %17.2Lf %17.2Lf",
        mem_shot.int_malloc_stats.requested_memory,
        mem_shot.int_malloc_stats.usable_allocation,
        mem_shot.int_malloc_stats.current_requested_memory,
        mem_shot.int_malloc_stats.current_usable_allocation,
        mem_shot.heap,
        mem_shot.mmap,
        mem_shot.total_dynamic,
        mem_shot.fragmentation.internal,
        mem_shot.fragmentation.external,
        mem_shot.fragmentation.total
        );

    if(fp == NULL) {printf("%s", str_buffer);}
    else {fprintf(fp, "%s\n", str_buffer);}
    free(str_buffer);
}

#endif