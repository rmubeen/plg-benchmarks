CC  := $(CC)
CXX := $(CXX)

CFLAGS  := -g3 -O0 -DMALLOC_PRELOAD

#####

TARGET = mallocWrappers1.so mallocWrappers.so
all : $(TARGET)

##### C++ Code

mallocWrappers1.so: mallocWrappers1.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ ../common/utilities.c -ldl #-DMALLOC_PRELOAD_DEBUG

mallocWrappers.so: mallocWrappers2.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ ../common/utilities.c -ldl #-DMALLOC_PRELOAD_DEBUG

#####

clean:
	rm -f $(TARGET)