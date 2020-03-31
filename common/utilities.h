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
	verbose_print prints given message to stdout if verbose flage is set to true
	input: message to print
	output: NONE
	effects: generates messages to stdout

*/
void verbose_print();
void print_file(char* fname);

char get_proc_status(int pid);

void init_ipc();
void end_ipc();

#endif