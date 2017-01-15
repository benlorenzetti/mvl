#ifndef PIV_GARRAY_H
#define PIV_GARRAY_H

/* piv_garray.h
*/

#include <string.h>
#include <stdio.h>
#include "piv_stdlib.h"


#define PIV_GEOMETRIC_BASE 3/2

#define PIV_GARRAY {0, {0, 0}, &piv_garray}
#define PIV_ARRAY(array) \
{(uintptr_t) (array + sizeof(array)), \
{(uintptr_t) (array + sizeof(array)), (uintptr_t)(array)}, \
&piv_garray}

uintptr_t piv_garray_add(uintptr_t, uintptr_t);
void piv_garray_cpy(piv_vec, piv_piece);
uintptr_t piv_garray_sbrk(piv_state*, uintptr_t);

piv_table const piv_garray = {
  piv_garray_add,
  piv_garray_cpy,
  piv_garray_sbrk
};

extern piv_table const piv_garray;

uintptr_t piv_garray_add(uintptr_t ptr, uintptr_t ptr_or_size) {
  return ptr - ptr_or_size;
}

void piv_garray_cpy(piv_vec dest, piv_piece src) {
  // Test for overlap (alias) of source and destination
  size_t data_size = piv_garray_add(src.begin, src.end);
  uintptr_t new_end = piv_garray_add(dest.begin, data_size);
  if(dest.begin > src.begin || new_end < src.end)
    memcpy((char*)dest.begin, (char*)src.end, data_size);
  else
    memmove((char*)dest.begin, (char*)src.end, data_size);
}

uintptr_t piv_garray_sbrk(piv_state* pie, uintptr_t size) {
  uintptr_t cap = piv_garray_add(pie->state.lvec.end, pie->state.lvec.begin);
  uintptr_t old_size = piv_garray_add(pie->state.end, pie->state.lvec.end);
  if(size + old_size > cap) {
printf("triggering resize\n");
    uintptr_t new = piv_realloc(pie->state.lvec.begin, size + old_size);
    piv_vec new_alloc;
    new_alloc.end = piv_garray_add(new, (-1) * size + old_size);
    new_alloc.begin = new;
printf("end = %d, begin = %d\n", new_alloc.end, new_alloc.begin);
    piv_garray_cpy(new_alloc, pie->piece);
    pie->end = piv_garray_add(new_alloc.end, size + old_size);
    pie->state.lvec = new_alloc;
  }
  else {
    pie->end = piv_garray_add(pie->state.end, size);
  }
  return size;  
}

#endif
