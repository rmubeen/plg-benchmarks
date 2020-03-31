#ifndef explicit_Malloc_Stats_H
#define explicit_Malloc_Stats_H

#include "../common/dataStructures.h"

/*
	parse_proc_maps reads file given by fname as a proc/pid/maps file and populates fields of T_memory_snapshot defined in common/dataStructures.h
	input: a char* string (name of file to be read and parsed)
	output: a variable of type T_memory_snapshot
	effects: may print error message(s)
*/
T_memory_snapshot parse_proc_maps(char* fname);

/*
	calculate_fragmentation takes a variable of type T_memory_snapshot and calculates fragmentation as per heap and mmap bytes allocated to process and
	memory requested/used by process
	input: a variable of type T_memory_snapshot which has memory statistics of process
	output: a variable of type T_fragmentation
	effects: NONE
*/
T_fragmentation calculate_fragmentation(T_memory_snapshot mem_shot);

/*
	incr_avg_fragmentation calculates incremental average of data of type T_fragmentation
	input: current average of type T_fragmentation, number of current instances and a new instance of T_fragmentation
	output: a variable of type T_fragmentation (new average)
	effects: NONE
*/
T_fragmentation incr_avg_fragmentation(T_fragmentation avg, int n, T_fragmentation new_frag);

/*
	incr_avg_mem_shot calculates incremental average of data of type T_memory_snapshot
	input: current average of type T_memory_snapshot, number of current instances and a new instance of T_memory_snapshot
	output: a variable of type T_memory_snapshot (new average)
	effects: NONE
*/
T_memory_snapshot incr_avg_mem_shot(T_memory_snapshot avg, int n, T_memory_snapshot new_mem);

/*
	incr_avg_int_mem_stats calculates incremental average of data of type T_int_malloc_stats
	input: current average of type T_int_malloc_stats, number of current instances and a new instance of T_int_malloc_stats
	output: a variable of type T_int_malloc_stats (new average)
	effects: NONE
*/
T_int_malloc_stats incr_avg_int_mem_stats(T_int_malloc_stats avg, int n, T_int_malloc_stats new_stats);

/*
	print_mem_stats_layout prints layout, in which memory statistics are printed, to a file if a pointer to it is given
	if NULL is provided as file pointer then layout is printed to stdout
	input: pointer to file or NULL
	output: NONE
	effects: may print to stdout or file stream
*/
void print_mem_stats_layout(FILE *fp);

/*
	print_mem_stats prints memory statistics to a file if a pointer to it is given
	if NULL is provided as file pointer then it prints to stdout
	input: pointer to file or NULL
	output: NONE
	effects: may print to stdout or file stream
*/
void print_mem_stats(T_memory_snapshot mem_shot, FILE *fp);

#endif