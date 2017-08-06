#include <stdio.h>
#include "pio.h"
#include "piv_string.h"
#include "pivlib.h"
#include "array.h"

#define TEST_ARRAY_SIZE 50

int char_compare(Nint a, Nint b) {
//    Wchar* aptr = (Wchar*)a;
//    Wchar* bptr = (Wchar*)b;
//    printf("cmp '%c' to '%c'\n", *aptr, *bptr);
    return ((Wchar*)a)[0] - ((Wchar*)b)[0];
}

int int_compare(const Nint a, const Nint b) {
    return ((int*)a)[0] - ((int*)b)[0];
}

int main() {
    STRING_MAIN_ASSERT();
    ustr string1 = c2ustr("Hello World!");
    ustr string2 = c2ustr("Hello");
    pio_fprintf(stdout, "ustr string1 = %S\n", string1);
    pio_fprintf(stdout, "ustr string2 = %S\n\n", string2);

    Wchar* str1 = ustr_get_str(&string1);
    Wchar* str2 = ustr_get_str(&string2);
    pio_fprintf(stdout, "Wchar* str1 = %s\n", str1);
    pio_fprintf(stdout, "Wchar* str2 = %s\n", str2);
    printf("&str1 = %lu\n&str2 = %lu\n\n", (Nint)str1, (Nint)str2);


    rvec vec1 = ustr_get_rvec(&string1);
    rvec vec2 = ustr_get_rvec(&string2);
    printf("vec1 = (%lu, %lu]\n", vec1.rvec.zero, vec1.rref);
    printf("vec2 = (%lu, %lu]\n", vec2.rvec.zero, vec2.rref);

    printf("rvec_cmp(vec1, vec2, 1, char_compare) = ");
    printf("%d\n\n", rvec_cmp(vec1.rvec, vec2.rvec, 1, char_compare));

    ustr_free(string1);
    ustr_free(string2);

    array ar1 = ARRAY_INIT;
    int input_array[TEST_ARRAY_SIZE];
    for(int i = 0; i < TEST_ARRAY_SIZE; i++)
        input_array[i] = i/2;
    array_partback(&ar1.array, -sizeof(input_array));
    r2l_memcpy(ar1.array.zero, (Wint)input_array, TEST_ARRAY_SIZE, sizeof(int));

    int i = 0;
    if (TEST_ARRAY_SIZE)
    do i++, printf("ar1[%3.d] = %d\n", i, ((int*)ar1.array.zero)[-i]);
    while (i < TEST_ARRAY_SIZE);

    int search_key = 25;
    rvec ins_range;
    ins_range.rvec = array_rvec_bsearch(&search_key, ar1.rvec, -((Nint)sizeof(int)), int_compare);
    Nint ins_ptr = array_rvec_lsearch(&search_key, ar1.rvec, -((Nint)sizeof(int)), int_compare);
    printf("find/insertion point for key = %d is ar1[%ld] = %d\n", search_key, (-(ins_ptr - ar1.array.zero))/sizeof(int), ((int*)ins_ptr)[0]);
}
