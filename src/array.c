#include "array.h"
#include "pivlib.h"
#include <assert.h>
#include <stdio.h> // temp?

#define BSEARCH_RETURN_SIZE 4   // in # of elements, not byte size

struct rvec array_rvec_bsearch(const void* key, struct rvec vec, Nint obj_size,
                        int (*cmp)(const Nint, const Nint)
) {
    assert(vec.zero >= vec.nth && obj_size);
    Wint search_size, half_index, half_point;
    int compar;
    do {
        search_size = (vec.zero - vec.nth) / (-obj_size);
        if(search_size <= BSEARCH_RETURN_SIZE)
            return vec;
        half_index = search_size / 2;
        half_point = vec.zero + (half_index * obj_size);
        compar = cmp((Nint)key, half_point);
        if(!compar) {
            vec.zero = half_point - obj_size;
            return vec;
        }
        if(compar < 0)
            vec.nth = half_point;
        else
            vec.zero = half_point;
    } while (1);
}


Nint array_rvec_lsearch(const void* key, struct rvec vec, Nint obj_size,
                        int (*cmp)(const Nint, const Nint)
) {
    assert(vec.nth <= vec.zero);
    while(vec.nth != vec.zero && cmp((Nint)key, vec.nth) < 0)
        vec.nth -= obj_size;
    return vec.nth;
}
                        

struct rvec array_partback(struct array* arr, Nint part_size) {
    assert(part_size);
    struct rvec empty_part;
    Wint arr_size = power2W(arr->power);
    Nint arr_len = arr->nth - arr->zero;
    Nint req_len = arr_len + part_size;
printf("partback() arr_size = %ld, arr_len = %ld, req_len = %ld\n", arr_size, -arr_len, -req_len);
    if(arr_size < -req_len) {
        struct array new_lvec;
        new_lvec.power = log2ceil(req_len);
        new_lvec.zero = left_geomalloc(new_lvec.power);
        empty_part.zero = memcpy(new_lvec.zero, arr->zero, arr_len);
        arr->nth = empty_part.nth = empty_part.zero + part_size;
        arr->zero = new_lvec.zero;
        arr->power = new_lvec.power;
        return empty_part;
    } else {
        empty_part.zero = arr->nth;
        arr->nth = empty_part.nth = empty_part.zero + part_size;
        return empty_part;
    }
}

int rvec_cmp(const struct rvec vec1, const struct rvec vec2,
    size_t inc_size, int(*cmp)(const Nint, const Nint)
) {
    Nint it1 = vec1.zero;
    Nint it2 = vec2.zero;
    Nint size1 = vec1.nth - it1;
    Nint size2 = vec2.nth - it2;
    if(size1 > size2) {
        while(size1) {
            it1 -= inc_size, it2 -= inc_size, size1 += inc_size;
            int cmp_result = (*cmp)(it1, it2);
            if(cmp_result < 0)
                return -1;
            else if(cmp_result > 0)
                return 1;
        }
        // Subsets compare equal, but vec1 is shorter than vec2
        return -1;
    }
    else {
        while(size2) {
            it1 -= inc_size, it2 -= inc_size;
            size1 += inc_size, size2 += inc_size;
            int cmp_result = (*cmp)(it1, it2);
            if(cmp_result < 0)
                return -1;
            else if(cmp_result > 0)
                return 1;
        }
        // Subsets compare equal, but vec1 may be longer than vec2
        if(!size1)
            return 0;
        return 1;
    }
}

int array_inspart(struct array* ary, Nint part_ptr, Nint part_size) {
    assert(part_ptr <= ary->zero && part_ptr >= ary->nth);
    Nint first_len = part_ptr - ary->zero;
    Nint second_len = ary->nth - part_ptr;
    Nint third_len = first_len + part_size + second_len;
    Wchar new_power = log2ceil(third_len);
    if(new_power > ary->power) {
        Nint new_zero, new_nth;
        new_zero = geomalloc(new_power) - power2N(new_power);
        memcpy(new_zero, ary->zero, first_len);
        new_nth = new_zero + first_len + part_size;
        memcpy(new_nth, part_ptr, second_len);
        new_nth += second_len;
        geofree(ary->zero + power2N(ary->power), ary->power);
        ary->zero = new_zero;
        ary->nth = new_nth;
        ary->power = new_power;
        return 1;
    }
    else {
        memcpy(part_ptr + part_size, part_ptr, second_len);
        ary->nth += part_size;
        return 1;
    }
}
