#ifndef allocation_H
#define allocation_H

#include "../common/dataStructures.h"

T_alloc_state init_allocations();
int next_allocation(T_alloc_state* cur_state);
void end_allocations(T_alloc_state* cur_state);

#endif // allocation_H