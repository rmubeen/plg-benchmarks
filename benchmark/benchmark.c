#ifndef benchmark_C
#define benchmark_C
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <unistd.h> // execvpe
#include <string.h> // strtok
#include <errno.h> // execvp return
#include "signal.h"

#include "../common/utilities.h"
#include "explicitMallocStats.h"

void parseArguments(int argc, char* argv[]) {
	// initiate knobs with default values

	knobs.delay_time = 10;
	knobs.executable = malloc(50 * sizeof(char));
	knobs.executable[0] = '\0';
	knobs.execution_args_i = argc;
	knobs.output_file = NULL;

	knobs.verbose_flag = 0;
	knobs.sig_bound = 0;
	knobs.send_ppid = 0;

	// customize knobs as per command-line arguments
	for(int i_argv=1; (i_argv < argc) && (knobs.execution_args_i == argc); i_argv++) {
		char* key = strtok(argv[i_argv], "=");
		char* value = strtok(NULL, "=");

		if (strcmp (key, "-t") == 0) {knobs.delay_time = atoi(value); goto LoopEnd;}
		else if (strcmp(key, "-v") == 0) {knobs.verbose_flag = 1; verbose = 1; goto LoopEnd;}
		else if (strcmp(key, "-o") == 0) {knobs.output_file = malloc(100 * sizeof(char)); sprintf(knobs.output_file, "%s", value); goto LoopEnd;}
		else if (strcmp(key, "-sig") == 0) {knobs.sig_bound = 1; start_sig = 0; stop_sig = 0; goto LoopEnd;}
		else if (strcmp(key, "-ppid") == 0) {knobs.send_ppid = 1; goto LoopEnd;}
		else if (strcmp(key, "-e") == 0) {sprintf(knobs.executable, "%s", value); knobs.execution_args_i = i_argv + 1; break;}
		else if (strcmp(key, "-h") == 0) { goto UsageMessage;}
		else { goto UsageMessage;}

		LoopEnd:
		key = value = NULL;
	}

	if (knobs.executable[0] == '\0') {printf("ERR: executable not specified\n"); goto UsageMessage;}
	return;

	UsageMessage:
	printf("Usage options:\n\
	-t=: sets delay between two readings\n\
	-e=: path to executable followed by parameters\n\
	-o=: output file\n\
	-sig: bound to start and stop signals for reading memory usage of process\n\
	-v: verbose mode\n\
	-h: help\n");
	exit(1);
}

void set_pre_shot(int sig) {
	char* proc_maps_fname = malloc(50 * sizeof(char));
	sprintf(proc_maps_fname, "/proc/%d/maps", shmem->pid);

	exp_malloc_stats.pre_mem_shot = parse_proc_maps(proc_maps_fname);
	exp_malloc_stats.pre_mem_shot.int_malloc_stats = *shmem;
	exp_malloc_stats.pre_mem_shot.fragmentation = calculate_fragmentation(exp_malloc_stats.pre_mem_shot);
	start_sig = 1;

	kill(shmem->pid, SIGCONT);
	sprintf(verbose_buffer, "\nPre called\n"); verbose_print();
	free(proc_maps_fname);
}

void set_post_shot(int sig) {
	char* proc_maps_fname = malloc(50 * sizeof(char));
	sprintf(proc_maps_fname, "/proc/%d/maps", shmem->pid);

	exp_malloc_stats.post_mem_shot = parse_proc_maps(proc_maps_fname);
	exp_malloc_stats.post_mem_shot.int_malloc_stats = *shmem;
	exp_malloc_stats.post_mem_shot.fragmentation = calculate_fragmentation(exp_malloc_stats.post_mem_shot);
	stop_sig = 1;

	kill(shmem->pid, SIGCONT);
	sprintf(verbose_buffer, "\nPost called\n"); verbose_print();
	free(proc_maps_fname);
}

int start_process(int argc, char* argv[]) {
	int proc_argc = knobs.send_ppid == 0 ? argc-knobs.execution_args_i+2 : argc-knobs.execution_args_i+3;
	char* proc_argv[proc_argc];

	proc_argv[0] = knobs.executable;
	if(knobs.send_ppid != 0) {
		proc_argv[proc_argc-2] = malloc(50 * sizeof(char));
		sprintf(proc_argv[proc_argc-2], "-ppid=%d", getpid());
	}
	proc_argv[proc_argc-1] = NULL;
	char* proc_env[] = {NULL, NULL};
	proc_env[0] = malloc(500 * sizeof(char));
	sprintf(proc_env[0], "LD_PRELOAD=%s/../malloc/mallocWrappers.so", getcwd(verbose_buffer, 500));

	for (int argc_i = 1; argc_i < (proc_argc-1 - knobs.send_ppid); ++argc_i) {
		proc_argv[argc_i] = argv[argc_i + knobs.execution_args_i - 1];
	}

	sprintf(verbose_buffer, "Running command %d: ", proc_argc); verbose_print();
	for (int i = 0; i < proc_argc; ++i) {
		sprintf(verbose_buffer, "%s ", proc_argv[i]); verbose_print();
	}
	sprintf(verbose_buffer, "\n"); verbose_print();

	int pid = fork();
	if(pid == 0) {
		execvpe(proc_argv[0], proc_argv, proc_env);
		printf("ERR: execvpe: %s", proc_argv[0]);
		exit(1);
	} else if (pid < 0) {
		printf("ERR: fork: %s", proc_argv[0]);
	}
	shmem->pid = pid;
	shmem->ppid = getpid();

	return pid;
}

void benchmark_process(int pid) {
	char* proc_maps_fname = malloc(50 * sizeof(char));
	sprintf(proc_maps_fname, "/proc/%d/maps", pid);

	if(knobs.verbose_flag == 1) {print_mem_stats_layout(NULL);};
	char pid_status = get_proc_status(pid);
	while ((pid_status != 'Z') && (pid_status != 'X') && (pid_status != 'x')) {
 		if(knobs.sig_bound != 0){
 			if(stop_sig != 0) {printf("\nRecieved stop signal"); break;}
 		}
 		sprintf(verbose_buffer, "\rchild status: %c, reading counter: %d, array size: %d", pid_status, exp_malloc_stats.counter, exp_malloc_stats.size); verbose_print();
		if (exp_malloc_stats.counter >= exp_malloc_stats.size) {
			exp_malloc_stats.mem_shots = realloc(exp_malloc_stats.mem_shots, (exp_malloc_stats.size + 1000) * sizeof(T_memory_snapshot));
			exp_malloc_stats.size += 1000;
		}

		int count = exp_malloc_stats.counter;
		kill(pid, SIGSTOP);
		T_memory_snapshot cur_mem_shot = parse_proc_maps(proc_maps_fname);
		cur_mem_shot.int_malloc_stats = *shmem;
		kill(pid, SIGCONT);
		if ((cur_mem_shot.total_dynamic == 0) || (cur_mem_shot.int_malloc_stats.current_usable_allocation == 0)) { goto LOOP_END; }
		cur_mem_shot.fragmentation = calculate_fragmentation(cur_mem_shot);

		exp_malloc_stats.mem_shots[exp_malloc_stats.counter] = cur_mem_shot;
		exp_malloc_stats.counter += 1;
		if(count == 0) {
			exp_malloc_stats.comp_sys[0] = exp_malloc_stats.comp_sys[1] = cur_mem_shot;
			exp_malloc_stats.comp_frag[0] = exp_malloc_stats.comp_frag[1] = cur_mem_shot;
			exp_malloc_stats.avg_mem_shot = cur_mem_shot;
		} else {
			if(cur_mem_shot.total_dynamic < exp_malloc_stats.comp_sys[0].total_dynamic) { exp_malloc_stats.comp_sys[0] = cur_mem_shot; }
			else if(cur_mem_shot.total_dynamic > exp_malloc_stats.comp_sys[1].total_dynamic) { exp_malloc_stats.comp_sys[1] = cur_mem_shot; }

			if(cur_mem_shot.fragmentation.total < exp_malloc_stats.comp_frag[0].fragmentation.total) { exp_malloc_stats.comp_frag[0] = cur_mem_shot; }
			else if(cur_mem_shot.fragmentation.total > exp_malloc_stats.comp_frag[1].fragmentation.total) { exp_malloc_stats.comp_frag[1] = cur_mem_shot; }

			exp_malloc_stats.avg_mem_shot = incr_avg_mem_shot(exp_malloc_stats.avg_mem_shot, count, cur_mem_shot);
		}

		LOOP_END:
		if(knobs.verbose_flag == 1) { printf("\r"); print_mem_stats(exp_malloc_stats.mem_shots[count], NULL); printf(": CUR"); }
		usleep(knobs.delay_time);
		pid_status = get_proc_status(pid);
	}
	sprintf(verbose_buffer, "\n"); verbose_print();
	if (pid_status == 'Z') { sprintf(verbose_buffer, "Child process went Zombie\n"); verbose_print(); }

	free(proc_maps_fname); proc_maps_fname = NULL;
	return;
}

/*
	main function handles arguments, initiates main instance, and thread instances
	input: verbpse, min_obj_size, max_obj_size, obj_size_distribution, min_obj_life, max_obj_life, obj_life_distro, objs_per_thread, threads
	output: return code
	side effects: generates output to stdout
*/
int main(int argc, char* argv[]) {
	// initiate global variables
	init_ipc();
	shmem->ppid = getpid();
	shmem->MSG[0] = '\0';
	shmem->malloc_count = 0;
	shmem->free_count = 0;
	shmem->calloc_count = 0;
	shmem->realloc_count = 0;
	shmem->requested_memory = 0;
	shmem->usable_allocation = 0;
	shmem->current_requested_memory = 0;
	shmem->current_usable_allocation = 0;
	verbose = 0;
	verbose_buffer = (char*) malloc(1000 * sizeof(char));
	start_sig = 1;
	stop_sig = 0;
	exp_malloc_stats.mem_shots = malloc(1000 * sizeof(T_memory_snapshot));
	exp_malloc_stats.size = 1000;
	exp_malloc_stats.counter = 0;
	signal(SIGUSR1, set_pre_shot);
	signal(SIGUSR2, set_post_shot);

	// handle commandline arguments
	parseArguments(argc, argv);
	sprintf(verbose_buffer, "time: %d\nverbose: %s\nexecution args start: %d\n", knobs.delay_time, verbose? "true":"false", knobs.execution_args_i); verbose_print();

	// initiate main thread
	int pid = start_process(argc, argv);
 	sprintf(verbose_buffer, "process %d created by %d\n", pid, getpid()); verbose_print();

 	// perform memory benchmarking
 	if(knobs.sig_bound != 0) {
		printf("Waiting for start signal...\n");
 		while(start_sig == 0) {}
 		printf("Recieved start signal\n");
 	}
	benchmark_process(pid);

	// print results
	print_mem_stats_layout(NULL);
	print_mem_stats(exp_malloc_stats.pre_mem_shot, NULL); printf(": PRE\n");
	print_mem_stats(exp_malloc_stats.post_mem_shot, NULL); printf(": POST\n");
	print_mem_stats(exp_malloc_stats.avg_mem_shot, NULL); printf(": AVG\n");
	printf("w.r.t. system memory:\n");
	print_mem_stats(exp_malloc_stats.comp_sys[0], NULL); printf(": LOW\n");
	print_mem_stats(exp_malloc_stats.comp_sys[1], NULL); printf(": HIGH\n");
	printf("w.r.t. fragmentation:\n");
	print_mem_stats(exp_malloc_stats.comp_frag[0], NULL); printf(": LOW\n");
	print_mem_stats(exp_malloc_stats.comp_frag[1], NULL); printf(": HIGH\n");

	if (knobs.output_file != NULL) {
		printf("Dumping memory snapshots\n");

		FILE* fp = fopen(knobs.output_file, "w");

		print_mem_stats_layout(fp);
		print_mem_stats(exp_malloc_stats.pre_mem_shot, fp);
		print_mem_stats(exp_malloc_stats.post_mem_shot, fp);
		print_mem_stats(exp_malloc_stats.avg_mem_shot, fp);
		print_mem_stats(exp_malloc_stats.comp_sys[0], fp);
		print_mem_stats(exp_malloc_stats.comp_sys[1], fp);
		print_mem_stats(exp_malloc_stats.comp_frag[0], fp);
		print_mem_stats(exp_malloc_stats.comp_frag[1], fp);

		for (int i = 0; i < exp_malloc_stats.counter; ++i) {
			print_mem_stats(exp_malloc_stats.mem_shots[i], fp);
		}

		fclose(fp);
	}

	// end
 	end_ipc();
	return 0;
}

#endif