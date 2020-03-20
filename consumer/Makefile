CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0
BUILTINFLAGS := -DNDEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

#####

all : distributionGen.o fragmentationInt.o fragmentation finish

TARGET = distributionGen.o fragmentationInt.o fragmentation finish

##### C++ Code

distributionGen.o: distributionGen.cpp
	$(CXX) $(CFLAGS) -c distributionGen.cpp -o distributionGen.o

#####

fragmentationInt.o: fragmentationInt.c
	$(CC) $(CFLAGS) -c fragmentationInt.c -o fragmentationInt.o

fragmentation: fragmentationInt.o distributionGen.o
	$(CC) $(CFLAGS) fragmentationInt.o dataStructures.h utilities.c explicitMallocHooks.c distributionGen.o -DBUILTIN_MALLOC -o fragmentation

#fragmentation-hoard: fragmentationInt.o distributionGen.o
#	$(CC) $(CFLAGS) $(BUILTINFLAGS) fragmentationInt.o dataStructures.h utilities.c explicitMallocHooks.c distributionGen.o -I../../hoard/common -L../../hoard/src -lhoard -DBUILTIN_MALLOC -o fragmentation-hoard

#####

finish: fragmentation
	rm -f *.o

clean:
	rm -f $(TARGET)