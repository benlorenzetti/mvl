#ifndef STRING_H
#define STRING_H

#include "pivlib.h"
#include <stdlib.h>
#include <stdio.h>

#define STRING_PTR_SIZE 6
#define STRING_USTR_HDR_SIZE 16
#define STRING_USTR_LEN_OFFSET 8
#define STRING_PTR_MASK (((Wint)1 << STRING_PTR_SIZE*8) - 1)

#define STRING_MAIN_ASSERT()                                         \
do {                                                                 \
    int bits;                                                        \
    Nint ptr_to_stack = -((Nint)&bits);                              \
    bits = log2ceil(ptr_to_stack);                                   \
    int bytes = 0;                                                   \
    do {bytes++; bits -= 8;} while (bits > 0);                       \
    if(bytes > STRING_PTR_SIZE) {                                    \
    printf("String fatal error!! The top of stack is beyond ");      \
        printf("pointable range.\n(LOREN_IPSUM_POINTER_SIZE would ");\
        printf("need at least %d bytes to be total.)\n", bytes);     \
        exit(EXIT_FAILURE);                                          \
    }                                                                \
} while (0)

typedef struct ustruct {
    Wchar str[STRING_PTR_SIZE];
    Wchar status;
    Wchar span[STRING_USTR_HDR_SIZE - STRING_PTR_SIZE - 1];
} ustruct;

typedef struct ustr_farlen {
    Wchar padding[STRING_USTR_LEN_OFFSET];
    Nint rend;
} ustr_rend_offset;

typedef union ustr {
    Wchar* str;
    ustruct ustr;
    char bytes[STRING_USTR_HDR_SIZE];
    ustr_rend_offset rend_offset;
} ustr;

ustr c2ustr(const char *);
Wchar* ustr_get_str(const ustr *);
rvec ustr_get_rvec(const ustr *);
void ustr_free(ustr);
void strclear(ustr *);

#endif
