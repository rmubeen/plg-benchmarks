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
};
struct s_int_malloc_statistics* int_malloc_stats;

typedef struct s_pointers_data {
	void* ptr;
	size_t requested_memory;
} td_pointers_data;
td_pointers_data* pointers_data;


#endif