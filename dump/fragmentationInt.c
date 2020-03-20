/*
	Mode: C

	fragmentationInt.c

	Author:
	Created:
	Last Modified:
	Update:

	To Do:
		Better Arguments Management
		Take Distributions of Size and Lifetime of objects
		mmao stats

	Useful resources:
		mcheck, mtrace, malloc_stats, malloc_info(0, stdout), malloc_usable_size(obj_ptrs[0])), mallopt(), open_memstream, mallinfo
*/

#ifndef fragmentation_Int_C
#define fragmentation_Int_C
#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "utilities.h"
#include "distributionGen.h"
#include "explicitMallocHooks.h"
#include <sys/sysinfo.h>

/*
	main instance to benchmark internal fragmentation
	input: min_obj_size, max_obj_size, num_objects
	output: NONE
	effects: generates messages to stdput
*/

/*
void check_usabe_memory_limit(void* ptr_1, void* ptr_2) {
	sprintf(ptr_1, "012345678901234567890");
	sprintf(ptr_2, "01234567890123456789012345678901");

	printf("ptr 1: %s\nptr 2: %s\n", ptr_1, ptr_2);

	return;
}
*/

void instance() {
	srand(time(0));
	long int malloc_counter = 0;
	long int free_counter = 0;
	uniformDistMetaData* u_dist_size = uniformDistInit(knobs.min_obj_size, knobs.max_obj_size, knobs.obj_size_step);
	start_exp_malloc_stats();
	
	while (malloc_counter < knobs.num_objects) {
		size_t obj_size = next(u_dist_size);
		pointers_data[malloc_counter].ptr = exp_malloc_hook(obj_size);
		pointers_data[malloc_counter].requested_memory = obj_size;
		malloc_counter += 1;
		sprintf(verbose_buffer, "num_objects: %ld, malloc_counter: %ld, obj_size: %lu\n", knobs.num_objects, malloc_counter, obj_size);
		verbose_print();
	}
	update_exp_malloc_stats(); print_exp_malloc_stats();
	print_fragmentation(calculate_fragmentation());

	while (free_counter < malloc_counter) {
		size_t obj_size = malloc_usable_size(pointers_data[free_counter].ptr);
		exp_free_hook(pointers_data[free_counter].ptr, pointers_data[free_counter].requested_memory);
		free_counter += 1;
		sprintf(verbose_buffer, "malloc_counter: %ld, free_counter: %ld, obj_size: %lu\n", malloc_counter, free_counter, obj_size);
		verbose_print();
	}

	FuncInstanceEnd:
	free(u_dist_size); u_dist_size = NULL;
}

/*
	main function handles arguments, initiates main instance, and thread instances
	input: verbpse, min_obj_size, max_obj_size, obj_size_distribution, min_obj_life, max_obj_life, obj_life_distro, objs_per_thread, threads
	output: return code
	side effects: generates output to stdout
*/
int main(int argc, char* argv[]) {
	// handle commandline arguments
	parseArguments(argc, argv);
	pointers_data = malloc(knobs.num_objects * sizeof(td_pointers_data));
	printf("verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects);

	// initiate main thread
	instance();
//	printf("verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects);

	return 0;
}

#endif