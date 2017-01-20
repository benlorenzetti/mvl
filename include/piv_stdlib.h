#ifndef PIV_STDLIB_H
#define PIV_STDLIB_H

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


#include "pstdint.h"
#include <stdlib.h>

struct piv_table_s;

typedef struct {
  uintptr_t end;
  uintptr_t begin;
} piv_piece;

typedef struct {
  uintptr_t rend;
  piv_piece lvec;
} piv_3state;

typedef union {
  piv_piece lvec;
  uintptr_t rend;
} piv_2state;

struct piv_vec {
  uintptr_t end;
  uintptr_t begin;
  const struct piv_table_s* v_table;
};

struct piv_slice {
  uintptr_t end;
  uintptr_t begin;
  uintptr_t root;
  const struct piv_table_s* v_table;
};

typedef union {
  struct piv_vec structure;
  piv_piece rvec;
  piv_2state state;
  char *end;
} pivec;

typedef union {
  struct piv_slice structure;
  piv_3state state;
  piv_piece rvec;
  char* end;
} piv_slice;

typedef struct piv_table_s {
  uintptr_t (*const add) (uintptr_t, uintptr_t);
  piv_piece (*const inc) (piv_piece, const size_t);
  void (*const cpy) (piv_piece, piv_piece); // (left vec dest, rvec src)
  size_t (*const sbrk) (piv_3state*, size_t);
  piv_piece (*const remove) (piv_3state*, piv_piece);
} piv_table;

uintptr_t piv_malloc(size_t size) {
  void* new = malloc(size);
  return (new == NULL) ? 0 : (uintptr_t) new;
}

void piv_free(uintptr_t ptr) {
  free((void*) ptr);
}

#define PIV_SLICE_XT(type) \
union { \
  struct piv_slice structure; \
  piv_3state state; \
  piv_piece rvec; \
  type *end; \
}

#define PIVEC_XT(type) \
union { \
  struct piv_vec structure; \
  piv_2state state; \
  piv_piece rvec; \
  piv_piece lvec; \
  type *end; \
}

#define PIV_SLICE(slice) \
{slice.structure.rend, slice.state.lvec, slice.structure.vec.v_table};

#define PIVEC(type) \
{0, 0, &pique};

#define PIV_INC(slice) \
(slice.state.lvec = \
 slice.structure.v_table->inc(slice.state.lvec, sizeof(*slice.end)) \
)

#define PIV_DET(piece) \
((piece).end != (piece).begin)

#define PIV_EMPTY(slice) \
(slice.rvec.end == slice.rvec.begin)

#define PIV_SIZE(slice) ( \
(size_t) ((slice.structure.v_table->add(slice.rvec.begin, slice.rvec.end)) \
/ sizeof(*slice.end)))

#define PIV_CAPACITY(slice) ( \
(size_t) ((slice.structure.v_table->add(slice.state.lvec.end, \
slice.state.lvec.begin)) / sizeof(*slice.end)))

#endif
