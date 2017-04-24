#include <stdio.h>
#include "loren_ipsum.h"

int main() {
	uintptr_t x = 129;
	printf("floor * log2(x) = %d\n", floor_log2(x));
	printf("ceiling * log2(x) = %d\n", ceiling_log2(x));
	printf("alloc_size required = %d\n", power2(ceiling_log2(x)));
}
