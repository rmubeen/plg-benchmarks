#ifndef explicit_Malloc_Stats_H
#define explicit_Malloc_Stats_H

#include "../common/dataStructures.h"

T_memory_snapshot parse_proc_maps(char* fname);
T_fragmentation calculate_fragmentation(T_memory_snapshot mem_shot);

T_fragmentation incr_avg_fragmentation(T_fragmentation avg, int n, T_fragmentation new_frag);
T_memory_snapshot incr_avg_mem_shot(T_memory_snapshot avg, int n, T_memory_snapshot new_mem); // returns: ((avg * n) + new_mem) / (n + 1)
T_int_malloc_stats incr_avg_int_mem_stats(T_int_malloc_stats avg, int n, T_int_malloc_stats new_stats);

void print_mem_stats_layout(FILE *fp);
void print_mem_stats(T_memory_snapshot mem_shot, FILE *fp);

#endif