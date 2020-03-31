CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0 -DMALLOC_PRELOAD

#####

TARGET = mallocWrappers.so
all : $(TARGET)

##### C++ Code

mallocWrappers.so: mallocWrappers.c
	$(CC) $(CFLAGS) -DMALLOC_PRELOAD_VERBOSE -shared -fPIC -o $@ $^ ../common/utilities.c -ldl

#####

clean:
	rm -f $(TARGET)