#include <stdio.h>
#include <stdlib.h>

int main() {

	size_t size = 100 * sizeof(char);
//	puts("before allocation\n");
//	printf("Before string allocation of size %ld\n", size);

	char* str = malloc(size);
	puts("after allocation\n");
	printf("String allocation of size %ld: %p %ld\n", size, str, malloc_usable_size(str));
/* 	str += (size_t)1;
 	printf("String allocation of size %ld: %p %ld %p\n", size, str, sizeof(size_t), (void*)((size_t)str));
 	str += (size_t)16;
	printf("String allocation of size %ld: %p %ld %p\n", size, str, (size_t)str, (void*)((size_t)str));
 	str -= (size_t)17;
	printf("String allocation of size %ld: %p %ld %p\n", size, str, (size_t)str, (void*)((size_t)str));
	free(str);

	str = malloc(size);
	printf("String allocation of size %ld: %x\n", size, str);
	free(str);

	str = calloc(100, sizeof(char));
	printf("String allocation of size %ld: %x\n", size, str);
	free(str);

	str = realloc(str, size + 50);
	printf("String allocation of size %ld: %x\n", size+50, str);
	free(str);

	str = realloc(str, size - 50);
	printf("String allocation of size %ld: %x\n", size-50, str);
	free(str);
*/
	return 0;
}