CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0 -DCONSUMER
BUILTINFLAGS := -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

#####

TARGET = distributionGen.o consumer.o consumer finish
all : $(TARGET)

##### C++ Code

distributionGen.o: distributionGen.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

#####

consumer.o: consumer.c
	$(CC) $(CFLAGS) -c -o $@ $^

consumer: consumer.o distributionGen.o
	$(CC) $(CFLAGS) -o $@ $^ ../common/utilities.c

#consumer-hoard: consumer.o distributionGen.o
#	$(CC) $(CFLAGS) $(BUILTINFLAGS) consumer.o dataStructures.h utilities.c distributionGen.o -I../../hoard/common -L../../hoard/src -lhoard -DBUILTIN_MALLOC -o consumer-hoard

#####

finish: consumer
	rm -f *.o

clean:
	rm -f *.o $(TARGET)