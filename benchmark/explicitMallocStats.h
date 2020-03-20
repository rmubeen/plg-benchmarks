/*
	Mode: C

	explicitMallocStats.h

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef explicit_Malloc_Stats_H
#define explicit_Malloc_Stats_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "dataStructures.h"

memory_snapshot parse_proc_maps(char* fname);
fragmentation calculate_fragmentation();

//void print_memory_snapshot(memory_snapshot ms);
//void print_exp_malloc_stats();
//void print_fragmentation(fragmentation frag);

#endif