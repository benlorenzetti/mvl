#ifndef PIVLIB_ARCH_H
#define PIVLIB_ARCH_H

#include <stdint.h>     // uintptr_t, intptr_t
#include <unistd.h>     // sbrk(), int getpagesize()
#include <assert.h>     // assert()

#define PIV_PAGE_SIZE (getpagesize())


// #include "pstdint.h"





uintptr_t noarch_sbrk(int);

#endif
