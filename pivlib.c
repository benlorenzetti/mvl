#include "pivlib.h"
#include <assert.h>

#define PIV_MIN_SIZEOF_ARRAY_INDEX	24	// sizeof in bits
#define PIV_MAX_ARRAY_SIZE	(1 << PIV_MIN_SIZEOF_ARRAY_INDEX)

struct piv_geometric_alloc_obj {
	uintptr_t bottom, top;
	pi_slice available[PIV_MIN_SIZEOF_ARRAY_INDEX];
};
static struct piv_geometric_alloc_obj galloc_obj;



#include <stdio.h>
int main() {
	printf("Heap break point at t=1: %lu\n", noarch_sbrk(0));
	printf("Heap break point at t=2: %lu\n", noarch_sbrk(1000));
	printf("page size = %d\n", PIV_PAGE_SIZE);
	piW w = 513;
	printf("           w = %lu\n", w);
	printf("log2floor(w) = %d\n", pi_log2floor(w));
	printf(" log2ceil(w) = %d\n", pi_log2ceil(w));
	printf("req. size(w) = %lu\n", pi_power2(pi_log2ceil(w)));
}

char pi_log2floor(piW x) {
	char rot_count = -1;
	while(x)
		x /= 2, rot_count++;
	return rot_count;
}

char pi_log2ceil (piW x) {
	if(x == pi_power2(pi_log2floor(x)))
		return pi_log2floor(x);
	else
		return 1 + pi_log2floor(x);
}

piW pi_power2(char n) {
	// Assert {1 << n} won't overflow piW type (uintptr_t)
	assert(n < 8*((char)sizeof(piW)));
	if(n < 0)
		return 0;
	else
		return 1 << n;
}
