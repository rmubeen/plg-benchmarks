#ifndef fragmentation_Int_C
#define fragmentation_Int_C
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <time.h> // srand
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include "signal.h"

#include "allocation.h"
#include <sys/sysinfo.h>
#include "../common/utilities.h"
//#include "../malloc/mallocWrappers.h"

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

    knobs.ppid = -1;
	knobs.verbose_flag = 0;
    knobs.output_file = NULL;

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
		else if (strcmp(key, "-ppid") == 0) {knobs.ppid = atoi(value); goto LoopEnd;}
        else if (strcmp(key, "-o") == 0) { knobs.output_file = (char*) malloc(101 * sizeof(char)); strcpy(knobs.output_file, value); goto LoopEnd;}
		else if (strcmp(key, "-v") == 0) {knobs.verbose_flag = 1; verbose = 1; goto LoopEnd;}
		else if (strcmp(key, "-h") == 0) { goto UsageMessage;}
		else { printf("Invalid argument: %s%s\n", key, value); goto UsageMessage;}

		UsageMessage:
		printf("Usage options:\n\
        -minS    :  sets min object size\n\
        -maxS    :  sets max object size\n\
        -stepS   :  sets object size increment\n\
        -distroS :  sets object size distribution\n\
        -minL    :  sets min object life\n\
        -maxL    :  sets max object life\n\
        -stepL   :  sets object life increment\n\
        -distroL :  sets object life distribution\n\
        -objN    :  sets number of objects per thread\n\
        -threadN :  sets number of threads\n\
        -o       :  output file for post-mortem\n\
        -v       :  verbose mode\n\
        -h       :  help\n");
		exit(1);

		BreakLoop:
		key = value = NULL;
		break;

		LoopEnd:
		key = value = NULL;
	}
}

void consumer_instance(int pid, FILE* fout) {
	srand(time(0));
    T_alloc_state mem_allocation = init_allocations();
	if(knobs.ppid != -1) { kill(knobs.ppid, SIGUSR1); kill(pid, SIGSTOP); } // Notify benchmark about start of allocations

    while(next_allocation(&mem_allocation) != 0) {
        if(fout){
            print_file("/proc/self/maps", fout);
            fprintf(fout, "#PRELOAD_MALLOC_STATS %ld %ld %ld %ld %lld %lld %lld %lld\n",
                shmem->malloc_count,
                shmem->free_count,
                shmem->calloc_count,
                shmem->realloc_count,
                shmem->requested_memory,
                shmem->usable_allocation,
                shmem->current_requested_memory,
                shmem->current_usable_allocation);
        }
    }

	if(knobs.ppid != -1) { kill(knobs.ppid, SIGUSR2); kill(pid, SIGSTOP); } // Notify benchmark about stop of allocations
    end_allocations(&mem_allocation);
}

/*
	main function handles arguments, initiates main instance, and thread instances
	input: verbpse, min_obj_size, max_obj_size, obj_size_distribution, min_obj_life, max_obj_life, obj_life_distro, objs_per_thread, threads
	output: return code
	side effects: generates output to stdout
*/

int main(int argc, char* argv[]) {
	//initiate global variables
	verbose = 0;
	verbose_buffer = (char*) malloc(1000 * sizeof(char));

	// handle commandline arguments
	parseArguments(argc, argv);
    FILE* fout = (knobs.output_file) ? fopen(knobs.output_file, "w") : NULL;
    if(fout) { init_ipc(); }
	sprintf(verbose_buffer, "verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\nppid: %d\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects, knobs.ppid); verbose_print();

	// job
	consumer_instance(getpid(), fout);
	sprintf(verbose_buffer, "verbose: %s\nmin_obj_size: %lu\nmax_obj_size: %lu\nnum_objects: %ld\nppid: %d\n", verbose? "true":"false", knobs.min_obj_size, knobs.max_obj_size, knobs.num_objects, knobs.ppid); verbose_print();

	// end
    if(fout) { fclose(fout); end_ipc(); }
	free(verbose_buffer);

	return 0;
}

#endif