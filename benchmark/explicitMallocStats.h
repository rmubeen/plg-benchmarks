#ifndef explicit_Malloc_Stats_H
#define explicit_Malloc_Stats_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "../common/dataStructures.h"

memory_snapshot parse_proc_maps(char* fname);
t_fragmentation calculate_fragmentation(memory_snapshot mem_shot);

memory_snapshot incr_avg_mem_shot(memory_snapshot avg, int n, memory_snapshot new_mem); // returns: ((avg * n) + new_mem) / (n + 1)
t_int_malloc_stats incr_avg_int_mem_stats(t_int_malloc_stats avg, int n, t_int_malloc_stats new_stats);
t_fragmentation incr_avg_fragmentation(t_fragmentation avg, int n, t_fragmentation new_frag);

void print_mem_stats_layout(FILE *fp);
void print_mem_stats(memory_snapshot mem_shot, FILE *fp);

#endif