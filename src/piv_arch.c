#include <stdint.h>     // uintptr_t, intptr_t
#include <unistd.h>     // sbrk(), int getpagesize()
#include <assert.h>     // assert()
#include "piv_arch.h"

uintptr_t noarch_sbrk(int inc) {
	assert(inc >= INTPTR_MIN && inc <= INTPTR_MAX);
	intptr_t inc_size = inc;
	return (uintptr_t) sbrk(inc_size);
}

