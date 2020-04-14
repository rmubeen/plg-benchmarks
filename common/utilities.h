#ifndef utilities_H
#define utilities_H

#include "dataStructures.h"

#ifndef SHM_KEY
#define SHM_KEY 0x123457
#endif
int shmid;
T_int_malloc_stats* shmem;

#ifdef BENCHMARK
int start_sig;
int stop_sig;
#endif

int verbose;
char* verbose_buffer;

/*
	verbose_print prints given message to stdout if verbose flage is set to non-zero
	input: a char* string (message to be printed)
	output: NONE
	effects: printing to stdout
*/
void verbose_print();

/*
	print_file reads the file with name as fname and prints it to the given file stream unless pointer is NULL then it goes to stdout
	input: a char* string (name of the file to be printed), an output file pointer
	output: NONE
	effects: printing to stdout or given file stream
*/
void print_file(char* fname, FILE* fout);

/*
	get_proc_status reads /proc/<pid>/stat file for given pid and returns the status of the process with that pid
	input: an int (pid of process whose status is required)
	output: a char (status of the process  as per Linux conventions. see man proc)
	effects: may print error message(s) to stdout
*/
char get_proc_status(int pid);

/*
	init_ipc attaches(creates it if it is not already created) to a shared memory region by SHM_KEY for inter-process communication
	ipc is used to share malloc stats between malloc-wrappers and benchmark program
	input: NONE
	output: NONE
	effects: may change value of T_int_malloc_stats* shmem or print error message(s) to stdout
*/
void init_ipc();

/*
	end_ipc detaches ptocess from shared memory region pointed by T_int_malloc_stats* shmem and marks it for destruction
	input: NONE
	output: NONE
	effects: may print error message(s) to stdout
*/
void end_ipc();

#endif