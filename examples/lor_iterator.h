#ifndef LOR_ITERATOR_H
#define LOR_ITERATOR_H

/*  Copyright (c) 2016 Ben Lorenzetti
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

/********************_The Lor Vector Container Library_***********************/
/*
 *
 */

#include "lor_unique_string.h"
/*  needed for generating unique casting structs for list iteratation functions
 *  that won't confilict with (probably redundant) structs defined by other
 *  Lor data structure macros.
*/

typedef struct lor_iterator_random_access_struct {
  void* ptr;
  void* (*const next)(void*);
  void* (*const prev)(void*);
  void* (*const advance)(void*, unsigned int);
} lor_iterator_random_access_t;

typedef struct lor_iterator_forward_struct {
  void* ptr;
  void* (*const next)(void*);
} lor_iterator_forward_t;

typedef union lor_iterator_union {
  lor_iterator_random_access_t ra;
  lor_iterator_forward_t fwd;
} lor_iterator_t;

#define LOR_ITERATOR_DEFINE_RANDOM_ACCESS_TYPE(type) \
static void* type##_array_next(void *ptr) { \
  return ptr += sizeof(type); \
} \
static void* type##_array_prev(void *ptr) { \
  return ptr -= sizeof(type); \
} \
static void* type##_advance(void *ptr, unsigned int dist) { \
  return ptr += dist * sizeof(type); \
}

#define LOR_ITERATOR_DEFINE_FORWARD_ITERATOR_TYPE_2(type, unique_str) \
struct type##_forward_list_##unique_str { \
  type val; \
  type *next; \
}; \
static void* type##_next(void *ptr) { \
  return ((struct type##_forward_list_##unique_str *)ptr)->next; \
}

#define LOR_ITERATOR_DEFINE_FORWARD_ITERATOR_TYPE(type) \
LOR_ITERATOR_DEFINE_FORWARD_ITERATOR_TYPE_2(type, LOR_UNIQUE_COMPILER_STRING)

#define LOR_ITERATOR_RANDOM_ACCESS_ITERATOR(name, type) \
union { \
  union { \
    lor_iterator_random_access_t iterator_class; \
    lor_iterator_t generic_iterator; \
  } iterator; \
  type *ptr; \
} name

#define LOR_ITERATOR_RANDOM_ACCESS(type) \
{{{0, type##_array_next, type##_array_prev, type##_advance}}}

#define LOR_ITERATOR_FORWARD_ITERATOR(name, type) \
union { \
  lor_iterator_forward_t class; \
  type *ptr; \
} name = {{0, type##_next}}; \
name.ptr

#define lor_iterator_next(it) \
(it.iterator.iterator_class.ptr = it.iterator.iterator_class.next(it.iterator.iterator_class.ptr))


#endif
