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

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "dataStructures.h"

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

#endif