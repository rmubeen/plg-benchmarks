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

#include "dataStructures.h"

static char proc_maps_fname_pre[] = "/proc/";
static char proc_maps_fname_post[] = "/maps\0";

void init_exp_malloc_stats();
void start_exp_malloc_stats();
void update_exp_malloc_stats();

memory_snapshot parse_proc_maps(char* fname);

fragmentation calculate_fragmentation();

#endif