#include "piv_string.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "pivlib.h"

#include "pio.h" // temp debug

Wchar* ustr_get_str(const ustr *userspace_str) {
    Nint ptr;
    ptr = (Nint)userspace_str->str & STRING_PTR_MASK;
    if (!ptr)
        ptr = (Nint)userspace_str + sizeof(*userspace_str);
    return (Wchar*) ptr;
}

rvec ustr_get_rvec(const ustr* userspace_str) {
    rvec v1;
    v1.rvec.zero = (Nint)userspace_str->str & STRING_PTR_MASK;
    if (v1.rvec.zero)
        v1.rvec.nth = userspace_str->rend_offset.rend;
    else {
        v1.rvec.zero = (Nint)userspace_str + sizeof(*userspace_str);
        v1.rvec.nth = v1.rvec.zero - userspace_str->ustr.status;
    }
    return v1;
}

ustr c2ustr(const char* cstr) {
    ustr nustr;
    Wint len = 0;
    Nint i = sizeof(nustr.ustr.span);
    Wchar* cursor = (Wchar*)(&nustr) + sizeof(nustr);
    if (cstr)
    do *(--cursor) = cstr[len++];
    while (--i && cursor[0]);
    if (!cursor[0]) {
        nustr.str = 0; // Null
        nustr.ustr.status = len; // Negative changes Wint -> Nint
    }
    else {
        while(cstr[len++]);
        len *= -1; // negative to convert Wint len -> Nint len
        Wchar alloc_size = log2ceil(len);
        cursor = (Wchar*) left_geomalloc(alloc_size);
        if(!cursor) {
            printf("geomalloc failure in c2ustr(%s)\n", cstr);
            exit(EXIT_FAILURE);
        }
        r2l_memcpy((Nint)cursor, (Wint)cstr, -len, 1);
        nustr.str = (Wchar*) cursor;
        nustr.ustr.status = alloc_size;
        nustr.rend_offset.rend = ((Nint) cursor) + len;
   }
    return nustr;
}

void ustr_free(ustr string) {
    Wint str = (Wint)string.str & STRING_PTR_MASK;
    if(str)
        left_geofree((Nint)str, string.ustr.status);
}


