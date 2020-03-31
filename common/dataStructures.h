#ifndef data_Structures_H
#define data_Structures_H

#include <stdlib.h>

typedef struct S_int_malloc_statistics {
	char MSG[11];
	int ppid;
	int pid;

	unsigned long int malloc_count;
	unsigned long int free_count;
	unsigned long int calloc_count;
	unsigned long int realloc_count;

	unsigned long long requested_memory;
	unsigned long long usable_allocation;
	unsigned long long current_requested_memory;
	unsigned long long current_usable_allocation;
} T_int_malloc_stats;

#ifdef MALLOC_PRELOAD

typedef struct S__malloc_preload {
	void* ptr;
	size_t size;
} T__maloc_preload;

#endif // MALLOC_PRELOAD

#ifdef CONSUMER    // Data-structures for consumer

struct S_parameters {
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
	int ppid;
} knobs;

#endif    //Data-structures for benchmark
#ifdef BENCHMARK

struct S_parameters {
	int delay_time;
	char* executable;
	int execution_args_i;
	char* output_file;

	int verbose_flag;		// only 0 means false
	int sig_bound;
} knobs;

typedef struct S_fragmentation {
	long double alignment;
	long double bookkeeping; // NOT WORKING YET

	long double internal; // alignment + bookkeeping
	long double external;

	long double total;
	long long int ref_bytes;
} T_fragmentation;

typedef struct S_memory_snapshot {
	T_int_malloc_stats int_malloc_stats;

	unsigned long int text;

	unsigned long int heap;
	unsigned long int mmap_so;
	unsigned long int mmap;

	unsigned long int stack;
	unsigned long int vvar;
	unsigned long int vdso;
	unsigned long int vsyscall;

	unsigned long int unfigured;
	unsigned long int total_dynamic;

	T_fragmentation fragmentation;
} T_memory_snapshot;

struct S_exp_malloc_statistics {
	int counter;
	int size;

	T_memory_snapshot pre_mem_shot;
	T_memory_snapshot post_mem_shot;
	T_memory_snapshot* mem_shots;

	T_memory_snapshot avg_mem_shot;
	T_memory_snapshot comp_sys[2]; //index: 0 lowest, 1 highest
	T_memory_snapshot comp_frag[2]; //index: 0 lowest, 1 highest

} exp_malloc_stats;

#endif // CONSUMER

#endif // data_Structures_H