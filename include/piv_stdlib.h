#ifndef PIV_STDLIB_H
#define PIV_STDLIB_H

#include "pstdint.h"
#include <stdlib.h>

struct piv_table_s;

typedef union {
  char* ptr;
  uintptr_t uint;
} piv_ptr;

typedef struct {
  uintptr_t end;
  uintptr_t begin;
} piv_piece;

typedef struct {
  uintptr_t end;
  uintptr_t begin;
} piv_vec;

typedef struct {
  uintptr_t end;
  piv_vec lvec;
} piv_state_s;

typedef union {
  piv_state_s state;
  piv_vec rvec;
  piv_piece piece;
  uintptr_t end;
  char* ptr;
} piv_state;

typedef struct {
  uintptr_t end;
  piv_vec begin;
  const struct piv_table_s* v_table;
} piv_slice_s;

typedef union {
  piv_slice_s slice;
  piv_state state;
  piv_vec rvec;
  piv_piece piece;
  uintptr_t end;
  char* ptr;
} piv_slice;

typedef struct piv_table_s {
  uintptr_t (*const add) (uintptr_t, uintptr_t);
  piv_piece (*const inc) (piv_state*, uintptr_t);
  void (*const cpy) (piv_vec, piv_piece); // (left vec dest, rvec src)
  uintptr_t (*const sbrk) (piv_state*, uintptr_t);
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
  piv_slice_s slice; \
  piv_state state; \
  piv_vec rvec; \
  piv_piece piece; \
  uintptr_t end; \
  type *ptr; \
}



#define PIV_EMPTY(pie) \
(pie.slice.piece.end == pie.slice.piece.begin)

#define PIV_SIZE(pie) ( \
(pie.slice.v_table->add(pie.piece.begin, pie.end)) \
/ sizeof(*pie.ptr))

#define PIV_CAPACITY(pie) ( \
(pie.slice.v_table->add(pie.state.state.lvec.end, \
pie.state.state.lvec.begin)) / sizeof(*pie.ptr))

#endif
