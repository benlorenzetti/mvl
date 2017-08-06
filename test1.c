#include "pivlib.h"
#include <stdio.h>
#include "pio.h"
#include "piv_string.h"
#include <stdint.h>

int main() {
    STRING_MAIN_ASSERT();

    Nint testptr = 0;
    printf("Max unsigned pointer value = %lu\n", UINTPTR_MAX);
    printf("              uintptr_test = %lu\n", testptr -1);

    Nint x = -129;
    printf("x = %ld\n", x);
    printf("floor = %d\n", log2floor(-x));
    printf("ceil = %d\n", log2ceil(x));
    printf("power2(ceil) = %ld\n", power2N(log2ceil(x)));

    Wchar array1[5] = {'e', 'd', 'c', 'b', 0};
    Wchar array2[5];
    Nint ar1 = (Nint)array1 + 5;
    Nint ar2 = (Nint)array2 + 5;
    memcpy(ar2, ar1, -5);
    printf("array2 = %s\n", array2);


    Wchar str1[5] = {0, 'd', 'c', 'b', 'a'};
    Wchar* str2 = &str1[5];
    pio_fprintf(stdout, "string test = %s, int test = %d\n", str2, 27); 

    // Test c2ustr() constructor and sfree() destructor
    ustr str3;
    str3 = c2ustr("xyz");
    Wchar* ptr = (Wchar*)&str3;
    pio_fprintf(stdout, "c2ustr(\"xyz\") = %S\n", str3);
    ustr_free(str3);
    str3 = c2ustr("1234567890");
    pio_fprintf(stdout, "c2ustr(\"1234567890\") = %S\n", str3);
    ustr_free(str3);
}
