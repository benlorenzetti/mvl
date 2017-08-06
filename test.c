#include "pivlib.h"
#include <stdio.h>

int main() {
    slice s1 = {99, 0, 128};
    s1.slice.rref = 99;
    s1.slice.lvec.zero = 0;
    s1.slice.lvec.size = 7; // 2^7 = 128

    printf(".slice.rref = %lu\n", s1.slice.rref);
    printf(".slice.lvec.zero = %lu\n", s1.slice.lvec.zero);
    printf(".slice.lvec.size = %lu\n", s1.slice.lvec.size);
    printf(".rvec.nth = %lu\n", s1.rvec.nth);
    printf(".rvec.zero = %lu\n", s1.rvec.zero);
    printf(".rref = %lu\n", s1.rref);

    printf("log2ceil(128) = %d\n", log2ceil(-128));
    printf("log2ceil(129) = %d\n", log2ceil(-129));
    printf("log2floor(256) = %d\n", log2floor(256));
    printf("log2floor(255) = %d\n", log2floor(256));
}
