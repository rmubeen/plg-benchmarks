/*
	Mode: C

	utilities.h

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef utilities_H
#define utilities_H

#include "dataStructures.h"

#ifndef SHM_KEY
#define SHM_KEY 0x123457
#endif

int verbose;
char* verbose_buffer;

int shmid;
struct s_int_malloc_statistics* shmem;

/*
	verbose_print prints given message to stdout if verbose flage is set to true
	input: message to print
	output: NONE
	effects: generates messages to stdout

*/
void verbose_print();
void print_file(char* fname);

void init_ipc();
void end_ipc();

#endif