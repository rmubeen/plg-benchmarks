CC  := $(CC)
CXX := $(CXX)
CFLAGS  := -g3 -O0 -DBENCHMARK

#####

all : benchmark
TARGET = benchmark

#####

benchmark: benchmark.c
	$(CC) $(CFLAGS) -o $@ $^ ../common/utilities.c explicitMallocStats.c

#####

clean:
	rm -f $(TARGET)