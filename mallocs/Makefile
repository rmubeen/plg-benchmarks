### From Hoard Benchmarks/Makefile.in
CC  := $(CC)
CXX := $(CXX)
#CXX=u++

CCFLAGS  := -g3 -O0 -DNDEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free
CXXFLAGS := -g3 -O0 -DNDEBUG -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free

###

#uCPPFLAGSDL=-DSTRUCT_MALLINFO_DECLARED -D_ext_malloc_

all: larson larson-hoard larson-dl larson-je larson-pt3 larson-rp larson-tbb larson-tc

TARGET = larson larson-hoard larson-dl larson-je larson-pt3 larson-rp larson-tbb larson-tc

##### CPP #####

larson: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp -o larson -lpthread

larson-hoard: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp  -I../../hoard/common -L../../hoard/src -lhoard -o larson-hoard -lpthread

larson-dl: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp ../../dlmalloc/malloc-2.8.6.c -o larson-dl -lpthread

larson-je: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp -L`jemalloc-config --libdir` -Wl,-rpath,`jemalloc-config --libdir` -ljemalloc `jemalloc-config --libs` -o larson-je -lpthread

larson-pt3: larson.cpp # ptmalloc3 built with make "linux-pthread"
	$(CXX) $(CXXFLAGS) larson.cpp ../../ptmalloc3/libptmalloc3.a -o larson-pt3 -lpthread

larson-rp: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp ../../rpmalloc-develop/bin/linux/release/x86-64/librpmallocwrap.so -o larson-rp -lpthread

larson-tbb: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp ../../tbbmalloc/build/linux_intel64_gcc_cc9.2.1_libc2.23_kernel4.15.0_release/libtbbmalloc_proxy.so.2 -o larson-tbb -lpthread

larson-tc: larson.cpp
	$(CXX) $(CXXFLAGS) larson.cpp /home/m3zulfiq/.local/lib/libtcmalloc_and_profiler.a -o larson-tc -lpthread

#larson-tlsf: larson.cpp
#	$(CXX) $(CXXFLAGS) larson.cpp ../../tlsf-246/src/tlsf.o -o larson-tlsf -lpthread

##### uC++ #####

#ularson: larson.cpp
#	$(uCPP) larson.cpp -I/home/m3zulfiq/.local/lib -o ularson

#ularson-hoard: larson.cpp
#	$(uCPP) larson.cpp -L../../hoard/src -lhoard -o ularson-hoard

#ularson-dl: larson.cpp
#	$(uCPP)  larson.cpp ../../dlmalloc/umalloc-2.8.6.c -o ularson-dl


#####

clean:
	rm -f $(TARGET)
