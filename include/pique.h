#ifndef PIQUE_H
#define PIQUE_H

/* pique.h
*/

#include <string.h>
#include <stdio.h>
#include "piv_stdlib.h"


#define PIQUE_GEOMETRIC_BASE 3/2

#define PIQUE_CONCAT(x, y) x ## y

#define PIQUE_DEFINE_XT(type) \
union PIQUE_CONCAT(type, pique_slice) \
{ \
  piv_slice_s slice; \
  piv_state state; \
  piv_vec rvec; \
  piv_piece piece; \
  uintptr_t end; \
  type *ptr; \
}; \
struct PIQUE_CONCAT(type, pique_pie) \
{ \
  union PIQUE_CONCAT(type, pique_slice) slice; \
  int (*const push) (struct PIQUE_CONCAT(type, pique_pie) *, type); \
}; \
int PIQUE_CONCAT(type, pique_push) ( \
  struct PIQUE_CONCAT(type, pique_pie) *pie, \
  type val) \
{ \
  if(pique.sbrk(&(pie->slice.state), sizeof(type)) < sizeof(type)) \
    return 0; \
  *pie->slice.ptr = val; \
  return 1; \
}

#define PIQUE_XT(type) struct PIQUE_CONCAT(type, pique_pie)
#define PIQUE(type) \
{ \
  {0, {0, 0}, &pique}, \
  PIQUE_CONCAT(type, pique_push) \
};


#define PI_ARRAY(array) \
{(uintptr_t) (array + sizeof(array)), \
{(uintptr_t) (array + sizeof(array)), (uintptr_t)(array)}, \
&pi_array}

uintptr_t pique_add(uintptr_t, uintptr_t);
piv_piece pique_inc(piv_state*, uintptr_t);
void pique_cpy(piv_vec, piv_piece);
uintptr_t pique_sbrk(piv_state*, uintptr_t);

piv_table const pique = {
  pique_add,
  pique_inc,
  pique_cpy,
  pique_sbrk
};

extern piv_table const pique;

uintptr_t pique_add(uintptr_t ptr, uintptr_t ptr_or_size) {
  return ptr - ptr_or_size;
}

piv_piece pique_inc(piv_state* pie, uintptr_t requested_size) {
  piv_piece piece = pie->piece;
  uintptr_t size = pique_add(pie->rvec.begin, pie->rvec.end);
  size = (size < requested_size) ? size : requested_size;
  pie->rvec.begin = pique_add(pie->rvec.begin, size);
  piece.end = pique_add(piece.begin, size);
  return piece;
}

void pique_cpy(piv_vec dest, piv_piece src) {
  // Test for overlap (alias) of source and destination
  size_t data_size = pique_add(src.begin, src.end);
  uintptr_t new_end = pique_add(dest.begin, data_size);
  if(dest.begin > src.begin || new_end < src.end)
    memcpy((char*)dest.begin, (char*)src.end, data_size);
  else
    memmove((char*)dest.begin, (char*)src.end, data_size);
}

uintptr_t pique_sbrk(piv_state* pie, uintptr_t size) {
  uintptr_t cap = pique_add(pie->state.lvec.end, pie->state.lvec.begin);
  uintptr_t old_size = pique_add(pie->rvec.begin, pie->rvec.end);
  if(size + old_size > cap) {
    uintptr_t new_size = (old_size + size) * PIQUE_GEOMETRIC_BASE;
    uintptr_t new = piv_malloc(new_size);
    piv_vec new_alloc;
    new_alloc.begin = new;
    new_alloc.end = pique_add(new, (-1) * new_size);
    pique_cpy(new_alloc, pie->piece);
    piv_free(pie->state.lvec.begin);
    pie->state.lvec = new_alloc;
    pie->rvec.end = pique_add(pie->state.lvec.end, (size + old_size));
  }
  else {
    pie->end = pique_add(pie->state.end, size);
  }
  return size;  
}

#endif
