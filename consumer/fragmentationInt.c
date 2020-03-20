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
#include <stdlib.h>
#include <string.h>
#include <time.h> // srand
#include "malloc.h"
#include "signal.h"

#include "utilities.h"
#include "distributionGen.h"
#include "explicitMallocHooks.h"
#include <sys/sysinfo.h>

void parseArguments(int argc, char* argv[]) {
	// initiate knobs with default values

	knobs.min_obj_size = 2;
	knobs.max_obj_size = 2560;	// 1 Byte less than usual MMAP threshhold
	knobs.obj_size_step = 2;
	knobs.size_distro = (char*) malloc(10 * sizeof(char));
	knobs.size_distro = "uniform";

	knobs.min_obj_life = 1.0;
	knobs.max_obj_life = 100.0;
	knobs.obj_life_step = 1.0;
	knobs.life_distro = (char*) malloc(10 * sizeof(char));
	knobs.life_distro = "uniform";

	knobs.num_objects = 1000000;
	knobs.num_threads = 0;

	knobs.verbose_flag = 0;

	// customize knobs as per command-line arguments
	for(int i_argv=1; i_argv < argc; i_argv++) {
		char* key = strtok(argv[i_argv], "=");
		char* value = strtok(NULL, "=");

		if (strcmp (key, "-minS") == 0) {knobs.min_obj_size = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-maxS") == 0) {knobs.max_obj_size = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-stepS") == 0) {knobs.obj_size_step = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-distroS") == 0) {strcpy(knobs.size_distro, value); goto LoopEnd;}
		else if (strcmp(key, "-minL") == 0) {knobs.min_obj_life = atof(value); goto LoopEnd;}
		else if (strcmp(key, "-maxL") == 0) {knobs.max_obj_life = atof(value); goto LoopEnd;}
		else if (strcmp(key, "-stepL") == 0) {knobs.obj_life_step = atof(value); goto LoopEnd;}
		else if (strcmp(key, "-distroL") == 0) {strcpy(knobs.life_distro, value); goto LoopEnd;}
		else if (strcmp(key, "-objN") == 0) {knobs.num_objects = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-threadN") == 0) {knobs.num_threads = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-v") == 0) {knobs.verbose_flag = 1; verbose = 1; goto LoopEnd;}
		else if (strcmp(key, "-h") == 0) { goto UsageMessage;}
		else { printf("Invalid argument: %s%s\n", key, value); goto UsageMessage;}

		UsageMessage:
		printf("Usage options:\n\t-minS: sets min object size\n\t-maxS: sets max object size\n\t-stepS: sets object size increment\n\t\
-distroS: sets object size distribution\n\t-minL: sets min object life\n\t-maxL: sets max object life\n\t-stepL: sets object life increment\n\t\
-distroL: sets object life distribution\n\t-objN: sets number of objects per thread\n\t-threadN: sets number of threads\n\t-v: verbose mode\n\t-h: help\n");
		exit(1);

		BreakLoop:
		key = value = NULL;
		break;

		LoopEnd:
		key = value = NULL;
	}
}

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
	
	kill(shmem->ppid, SIGUSR1);
	kill(shmem->pid, SIGSTOP);
	if(verbose) {malloc_stats(); printf("\n");}
	while (malloc_counter < knobs.num_objects) {
		size_t obj_size = next(u_dist_size);
		pointers_data[malloc_counter].ptr = exp_malloc_hook(obj_size);
		pointers_data[malloc_counter].requested_memory = obj_size;
		malloc_counter += 1;
		sprintf(verbose_buffer, "num_objects: %ld, malloc_counter: %ld, obj_size: %lu\n", knobs.num_objects, malloc_counter, obj_size);	verbose_print();
	}

	if(verbose) {malloc_stats(); printf("\n");}
	while (free_counter < malloc_counter) {
		size_t obj_size = pointers_data[free_counter].requested_memory;
		exp_free_hook(pointers_data[free_counter].ptr, pointers_data[free_counter].requested_memory);
		free_counter += 1;
		sprintf(verbose_buffer, "malloc_counter: %ld, free_counter: %ld, obj_size: %lu\n", malloc_counter, free_counter, obj_size);	verbose_print();
	}
	kill(shmem->ppid, SIGUSR2);
	kill(shmem->pid, SIGSTOP);

	if(verbose) {malloc_stats(); printf("\n");}
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
	//initiate global variables
	init_ipc();
	verbose = 0;
	verbose_buffer = (char*) malloc(1000 * sizeof(char));
	int_malloc_stats = shmem;
	// handle commandline arguments
	parseArguments(argc, argv);
	pointers_data = malloc(knobs.num_objects * sizeof(td_pointers_data));
	sprintf(verbose_buffer, "verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects); verbose_print();

	// initiate main thread
	instance();
	sprintf(verbose_buffer, "verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects); verbose_print();

	end_ipc();
	free(verbose_buffer);
	free(pointers_data);
	return 0;
}

#endif