CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0 -DNDEBUG
BUILTINFLAGS := -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

#####

all : benchmark

TARGET = benchmark

##### C++ Code

benchmark: benchmark.c
	$(CC) $(CFLAGS) $(BUILTINFLAGS) benchmark.c ../common/utilities.c explicitMallocStats.c -o benchmark

#####

clean:
	rm -f $(TARGET)