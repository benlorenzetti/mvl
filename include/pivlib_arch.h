#ifndef PIVLIB_ARCH_H
#define PIVLIB_ARCH_H

#include <stdint.h>     // uintptr_t, intptr_t
#include <unistd.h>     // sbrk(), int getpagesize()
#include <assert.h>     // assert()

#define PIV_PAGE_SIZE (getpagesize())


// #include "pstdint.h"





uintptr_t noarch_sbrk(int);

uintptr_t noarch_sbrk(int inc) {
	assert(inc >= INTPTR_MIN && inc <= INTPTR_MAX);
	intptr_t inc_size = inc;
	return (uintptr_t) sbrk(inc_size);
}

#endif
