#ifndef allocation_C
#define allocation_C

#include <stdio.h>
#include <malloc.h>

#include "allocation.h"
#include "distributionGen.h"

T_alloc_state init_allocations() {
	T_alloc_state allocation_state;

	allocation_state.malloc_counter = 0;
	allocation_state.free_counter = 0;
	allocation_state.size_distro = uniformDistInit(knobs.min_obj_size, knobs.max_obj_size, knobs.obj_size_step);
	allocation_state.ptrs_array = malloc(knobs.num_objects * sizeof(void*));

	return allocation_state;
}

int next_allocation(T_alloc_state* cur_state) {

	if(cur_state->malloc_counter < knobs.num_objects) {
		size_t obj_size = next(cur_state->size_distro);
		cur_state->ptrs_array[cur_state->malloc_counter] = malloc(obj_size);
        if(knobs.verbose_flag) { printf("malloc counter: %ld free counter: %ld address: %p size: %ld usable size: %ld\n", cur_state->malloc_counter, cur_state->free_counter, cur_state->ptrs_array[cur_state->malloc_counter], obj_size, malloc_usable_size(cur_state->ptrs_array[cur_state->malloc_counter])); }

		cur_state->malloc_counter += 1;
		return 1;
	} else if(cur_state->free_counter < cur_state->malloc_counter) {
        if(knobs.verbose_flag) { printf("malloc counter: %ld free counter: %ld address: %p size: %ld\n", cur_state->malloc_counter, cur_state->free_counter, cur_state->ptrs_array[cur_state->free_counter], malloc_usable_size(cur_state->ptrs_array[cur_state->free_counter])); }
		free(cur_state->ptrs_array[cur_state->free_counter]);
        cur_state->ptrs_array[cur_state->free_counter] = NULL;

		cur_state->free_counter += 1;
		return 1;
	}

	return 0; // end of user required memory usage
}

void end_allocations(T_alloc_state* cur_state) {

	cur_state->malloc_counter = 0;
	cur_state->free_counter = 0;

	free(cur_state->size_distro);
	cur_state->size_distro = NULL;

	free(cur_state->ptrs_array);
	cur_state->ptrs_array = NULL;

	return;
}

#endif // allocation_C