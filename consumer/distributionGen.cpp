/*
	Mode: C++

	distributionGen.cpp

	Author:
	Created:
	Last Modified:
	Update:
*/

#ifndef distribution_Gen_CPP
#define distribution_Gen_CPP

#include <random>
#include <ctime>

using namespace std;

extern "C"
{

	#include "distributionGen.h"

	struct s_uniformDistMetaData {
		default_random_engine generator;
		uniform_int_distribution<size_t> distribution;

		size_t min_obj_size;
		size_t max_obj_size;
		size_t obj_size_step;
	};

	uniformDistMetaData* uniformDistInit(size_t min, size_t max, size_t step) {
		uniformDistMetaData* uniformDist = (uniformDistMetaData*) malloc(sizeof(uniformDistMetaData));

		srand(time(0));
		int num_buckets = ((max - min) / step) + 1;

		uniformDist->distribution.~uniform_int_distribution();
		uniformDist->distribution = uniform_int_distribution<size_t> (0, num_buckets-1);

		uniformDist->generator.~default_random_engine();
		uniformDist->generator = default_random_engine(time(0));

		uniformDist->min_obj_size = min;
		uniformDist->max_obj_size = max;
		uniformDist->obj_size_step = step;

		return uniformDist;
	}

	int next(uniformDistMetaData* uniformDist) {
		return uniformDist->min_obj_size + (uniformDist->obj_size_step * uniformDist->distribution(uniformDist->generator));
	}
}

#endif