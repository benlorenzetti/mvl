#ifndef PIQUE_H
#define PIQUE_H

/* 
 * Copyright (C) 2017 Ben Lorenzetti
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdio.h>
#include "piv_stdlib.h"


#define PIQUE_RESIZE_FACTOR 3/2

#define PIQUE_CONCAT(x, y) x ## y

#define PIQUE_DEFINE_XT(type) \
union PIQUE_CONCAT(type, pique_slice) \
{ \
  struct piv_slice structure; \
  piv_3state state; \
  piv_piece rvec; \
  type *end; \
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
  *pie->slice.end = val; \
  return 1; \
}

#define PIQUE_XT(type) struct PIQUE_CONCAT(type, pique_pie)
#define PIQUE(type) \
{ \
  {0, 0, 0, &pique}, \
  PIQUE_CONCAT(type, pique_push) \
};


#define PI_ARRAY(array) \
{(uintptr_t) (array + sizeof(array)), \
{(uintptr_t) (array + sizeof(array)), (uintptr_t)(array)}, \
&pi_array}

uintptr_t pique_add(uintptr_t, uintptr_t);
// to get rvec size use (.begin, .end); for lvecs, (.end, .begin)
piv_piece pique_inc(piv_piece*, const size_t);
void pique_cpy(piv_piece, piv_piece);
uintptr_t pique_sbrk(piv_3state*, size_t);

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

piv_piece pique_inc(piv_piece* lvec, const size_t req_size) {
  piv_piece piece;
  piece.begin = lvec->end;
  size_t size = pique_add(lvec->end, lvec->begin);
  size = (size < req_size) ? size : req_size;
  lvec->end = pique_add(lvec->end, size);
  piece.end = lvec->end;
  return piece;
}

void pique_cpy(piv_piece dest, piv_piece src) {
  // Test for overlap (alias) of source and destination
  size_t data_size = pique_add(src.begin, src.end);
  piv_piece rvec;
  rvec.begin = dest.end;
  rvec.end = pique_add(rvec.begin, data_size);
  if(rvec.end > src.begin || rvec.begin < src.end)
    memcpy((char*) rvec.end, (char*) src.end, data_size);
  else
    memmove((char*) rvec.end, (char*) src.end, data_size);
}

uintptr_t pique_sbrk(piv_3state* state, size_t size) {
  size_t capacity, old_size, new_size;
  capacity = pique_add(state->lvec.end, state->lvec.begin);
  old_size = (pique_add(state->lvec.end, state->rend));
  if(size + old_size > capacity) {
    new_size = ((old_size + size) * PIQUE_RESIZE_FACTOR) & ~(sizeof(int)-1);
    uintptr_t new = piv_malloc(new_size);
    piv_piece new_alloc, old_piece;
    old_piece.end = state->rend;
    old_piece.begin = state->lvec.end;
    new_alloc.begin = new;
    new_alloc.end = pique_add(new, (-1) * new_size);
    pique_cpy(new_alloc, old_piece);
    piv_free(state->lvec.begin);
    state->lvec = new_alloc;
    state->rend = pique_add(state->lvec.end, (size + old_size));
  }
  else {
    state->rend = pique_add(state->rend, size);
  }
  return size;  
}

#endif
