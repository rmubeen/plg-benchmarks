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

int verbose;
char* verbose_buffer;

void parseArguments(int argc, char* argv[]);

/*
	verbose_print prints given message to stdout if verbose flage is set to true
	input: message to print
	output: NONE
	effects: generates messages to stdout

*/
void verbose_print();

void print_exp_malloc_stats();

void print_memory_snapshot(memory_snapshot ms);

void print_file(char* fname);

#endif