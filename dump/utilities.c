/*
	Mode: C

	utilities.c

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef utilities_C
#define utilities_C

#include "stdio.h"
#include "string.h"

#include "utilities.h"

/*
	verbose_print prints given message to stdout if verbose flage is set to true
	input: message to print
	output: NONE
	effects: generates messages to stdout

*/
void verbose_print() {
	if (verbose == 1) {
		printf("%s", verbose_buffer);
		verbose_buffer[0] = '\0';
	}
}

void print_file(char* fname) {
	char buffer[1000];
	FILE* fptr = fopen(fname, "r");

	while(fgets(buffer, 1000, fptr) != NULL) {
		printf("%s", buffer);
	}

	FUNC_PRINT_FILE_END:
	fclose(fptr);
	free(fptr); fptr = NULL;
	return;
}

#endif