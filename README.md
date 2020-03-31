# plg-benchmarks

## benchmark
This directory contains main scripts used for reading memopry layout of program to be benchmarked and benchmark some features of memory allocator. It takes an executable as an argument to benchmark.

Features curently supported:
```
Fragmentation
```
*
cd benchmark

make

./benchmark -h
*
## common
This directory contains some simple function definitions and data structures which will be used by other programs.

## consumer
This directory contains a consumer program which can be used for custom memory allocation pattern.

*
cd consumer

make

./consumer -h
*
## malloc
This directory contains wrapper functions for malloc, calloc, realloc, and free.
These are pre-loaded with consumer or any executable given to benchmark program.