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
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include <sys/ipc.h>
#include <sys/shm.h>

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
	fclose(fptr); fptr = NULL;
	return;
}

void init_ipc() {
	shmid = shmget(SHM_KEY, sizeof(struct s_int_malloc_statistics), IPC_CREAT | 0666);
	if(shmid == -1) {
		error(0, errno, "ERR: IPC failed at shmget");
		exit(1);
	}

	shmem = shmat(shmid, NULL, 0);
	if (shmem == (void *) -1) {
		error(0, errno, "ERR: IPC failed at shmat");
		end_ipc();
		exit(1);
	}
}

void end_ipc() {
	if (shmdt(shmem) == -1) {
		error(0, errno, "ERR: IPC failed at shmdt");
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		error(0, errno, "ERR: IPC failed at shmctl");
	}
}

#endif