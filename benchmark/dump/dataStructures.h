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

struct s_parameters {
	int delay_time;
	char* executable;
	int execution_args_i;

	int verbose_flag;		// only 0 means false
} knobs;

struct s_int_malloc_statistics {
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
} int_malloc_stats;

typedef struct s_memory_snapshot {
	struct s_int_malloc_statistics int_malloc_stats;

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

struct s_exp_malloc_statistics {
	int counter;
	int size;

	memory_snapshot peak_mem_shot;
	memory_snapshot pre_mem_shot;
	memory_snapshot* mem_shots;
	memory_snapshot post_mem_shot;

} exp_malloc_stats;

typedef struct s_fragmentation {
	long long int alignment;
	long long int bookkeeping;
	long long int internal; // alignment + bookkeeping
	long long int external;

	long long int ref_bytes;
} fragmentation;

#endif