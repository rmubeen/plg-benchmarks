CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0 -DCONSUMER
BUILTINFLAGS := -DNDEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

#####

TARGET = distributionGen.o consumer.o consumer consumer-hoard finish
all : $(TARGET)

##### C++ Code

distributionGen.o: distributionGen.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

#####

consumer.o: consumer.c
	$(CC) $(CFLAGS) -c -o $@ $^

consumer: consumer.o distributionGen.o
	$(CC) $(CFLAGS) -o $@ $^ ../common/utilities.c allocation.c

consumer-hoard: consumer.o distributionGen.o
	$(CC) $(CFLAGS) $(BUILTINFLAGS) -o $@ $^ ../common/utilities.c allocation.c -I./mallocs/hoard-310320/src -L./mallocs/hoard-310320/src -lhoard -lpthread

#####

finish: consumer consumer-hoard
	rm -f *.o

clean:
	rm -f *.o $(TARGET)