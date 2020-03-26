#ifndef data_Structures_H
#define data_Structures_H

#include <stdlib.h>

typedef struct s_int_malloc_statistics {
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
} t_int_malloc_stats;
struct s_int_malloc_statistics* int_malloc_stats;

#ifdef CONSUMER    //Data-structures for consumer

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

typedef struct s_pointers_data {
	void* ptr;
	size_t requested_memory;
} td_pointers_data;
td_pointers_data* pointers_data;

#else    //Data-structures for benchmark

struct s_parameters {
	int delay_time;
	char* executable;
	int execution_args_i;
	char* output_file;

	int verbose_flag;		// only 0 means false
	int sig_bound;
} knobs;

typedef struct s_fragmentation {
	long double alignment;
	long double bookkeeping; // NOT WORKING YET

	long double internal; // alignment + bookkeeping
	long double external;

	long double total;
	long long int ref_bytes;
} t_fragmentation;

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
	unsigned long int total_dynamic;

	t_fragmentation fragmentation;
} memory_snapshot;

struct s_exp_malloc_statistics {
	int counter;
	int size;

	memory_snapshot pre_mem_shot;
	memory_snapshot post_mem_shot;
	memory_snapshot* mem_shots;

	memory_snapshot avg_mem_shot;
	memory_snapshot comp_sys[2]; //index: 0 lowest, 1 highest
	memory_snapshot comp_frag[2]; //index: 0 lowest, 1 highest

} exp_malloc_stats;

#endif //CONSUMER

#endif //data_Structures_H