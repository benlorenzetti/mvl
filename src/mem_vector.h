#ifndef MEM_VECTOR_H
#define MEM_VECTOR_H
/*  
 *  Memory Vector Library
 *  Copyright (C) 2016 Ben Lorenzetti
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the 
 *  Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  email: ben.lorenzetti@gmail.com
*/

/*  ____Memory Vectors____
 *  A memory vector is "fat pointer" made up of [a pointer to an element in
 *  memory + a set of instructions for advancing to neighboring elements].
 *  
 *  Memory vectors are like iterators from the C++ STL (which has a beautiful
 *  design BTW). They allow one to write generic data structures and generic
 *  algorithms and then combine the two for any user-defined type. For example,
 *  a linear search algorithm could run over an array or a linked list just by
 *  using a vector with an appropriate increment function. 
 *
 *  The three functions that must be defined for a memory vector are an
 *  incrementer, a decrementer, and an adder for advancing a arbitrary number
 *  of elements in either direction. These functions are constant and must be
 *  initialized at compile time so a good compiler can reduce call overhead.
*/ 

struct mem_vector_s{
  void* end;
  void* (*const inc)(void*);
  void* (*const dec)(void*);
  void* (*const add)(void*, int);
};

struct mem_slice_s{
  void* end;
  void* (*const inc)(void*);
  void* (*const dec)(void*);
  void* (*const add)(void*, int);
  void* begin;
};

/*  ____Slice Structures____
 *  The simplest data structure is a slice, so named because it consists of two
 *  memory vectors emanating from a single block of metadata. Valid data exists
 *  from the begin pointer up to one-memory-block-before-the-end vector. 
 *  Algorithms can do accessing, searching, swapping, and assigning on a slice
 *  but there are no methods that change the slice's size or memory footprint.
 *
 *  All other data structures in the Memory Vector Library are polymorphic
 *  with slices, and slices with vectors; i.e. first 5 members of any data
 *  structure's metadata are (in order) end, inc, dec, add, and begin. Since
 *  slices are a subtype of every data structure and have no methods with
 *  memory-modifying side effects, it is always safe to reference another
 *  data structure through a slice or a vector.
 *  
 *  To facilitate reference taking, every data structure _t type is defined as
 *  a union with slice and vector structs and a generic void* end pointer.
 *  Memory vectors are also unioned with a "degenerate slice" so that ".end"
 *  ".slice.begin" and "slice.end" are always valid member access paths for
 *  macros
*/

typedef union mem_slice_u {
  struct mem_slice_s slice;
  struct mem_vector_s vector;
  void* ptr;
  void* end;
} mem_slice_t;

typedef union mem_vector_u {
  struct mem_vector_s vector;
  union{ void* end; void* begin; } slice; // a degenerate slice
  void* ptr;
  void* end;
} mem_vector_t;

/*  ____Iteration____
 *  Memory vectors are more or less equivalent to iterators in the C++ STL.
 *  The classic use case is a for loop scan through a linked list.
 *  The list provides an at() method for accessing elements by index, but
 *  accessing element n is itself an O(n) operation, so a naive loop is O(n^2).
 *  
 *      int i;
 *      for(i = 0; i<list1.size; i++)
 *          int value = list.at(&list1, i);
 *
 *  Memory vectors allow you to bring this loop back to O(n) complexity by
 *  switching from O(n) at() to O(1) mem_vector_next().
 *
 *      mem_vector_t i = MEM_VECTOR(list1);
 *      for(i.ptr = list1.slice.begin; i.ptr < list1.end; mem_vector_next(i))
 *          int value = *((int*)(i.ptr));
 *  
 *  Three macros are defined for calling inc, dec, and add for memory vector
 *  arithmetic: mem_vector_next(), mem_vector_prev() and mem_vector_advance().
*/

#define mem_vector_next(mem_vec) \
(mem_vec.vector.end = mem_vec.vector.inc(mem_vec.vector.end))

#define mem_vector_prev(mem_vec) \
(mem_vec.vector.end = mem_vec.vector.dec(mem_vec.vector.end))

#define mem_vector_advance(mem_vec, dist) \
(mem_vec.vector.end = mem_vec.vector.add(mem_vec.vector.end, dist))

/*  
 *  In the above example, there is ugly casting to retrieve the value from the
 *  vector's generic void* ptr. This can be avoided by using a type template.
 *
 *      mem_vector_T(int) i = MEM_VECTOR(list1);
 *      for(i.ptr = list1.slice.begin; i.ptr < list1.end; mem_vector_next(i))
 *          int value = *i.ptr;
*/

#define mem_vector_T(type) \
union { \
  struct mem_vector_s vector; \
  union{ void* begin; void* end; } slice; \
  type *ptr; \
}

#define mem_slice_T(type) \
union { \
  struct mem_slice_s slice; \
  struct mem_vector_s vector; \
  type *ptr; \
}

/*
 *  The above example can be simplified yet again with the FOR_EACH macro.
 *  (assuming that list1 was declared with template mem_list_T(int))
 *
 *      MEM_VECTOR_FOR_EACH(&list1) {
 *          int value = *list1.ptr;
 *      }
 *
 *  Note that the FOR_EACH macro should not be used with break or goto
 *  statements that disrupt the control flow because the data structure's
 *  internal end vector is used as the index variable and it is not restored
 *  until the loop exits normally.
*/

#define MEM_VECTOR_FOR_EACH(data_structure) \
void* end_copy = slice_ptr.end; \
for(data_structure.end = data_structure.slice.begin; \
    data_structure.end < end_copy; \
    mem_vector_next(data_structure) \
)

#define MEM_VECTOR(existing_data_structure) \
{existing_data_structure.slice.end, \
 existing_data_structure.vector.inc, \
 existing_data_structure.vector.dec, \
 existing_data_structure.vector.add};
 
#define MEM_SLICE(existing_data_structure) \
{existing_data_structure.vector.begin, \
 existing_data_structure.vector.inc, \
 existing_data_structure.vector.dec, \
 existing_data_structure.vector.add, \
 existing_data_structure.vector.end};

const mem_slice_t* mem_splice(mem_slice_t *dest, void *begin, void *end) {
  dest->slice.begin = begin;
  dest->slice.end = end;
  return dest;
}

#define MEM_SPLICE(dest_ptr, begin_data_structure, end_ptr) \
mem_splice(dest_ptr, begin_data_structure.slice.begin, end_ptr)

#define MEM_VECTOR_RANDOM_ACCESS_T(type) \
static void* type##_array_inc(void *ptr) { \
  return ptr += sizeof(type); \
} \
static void* type##_array_dec(void *ptr) { \
  return ptr -= sizeof(type); \
} \
static void* type##_add(void *ptr, int dist) { \
  return ptr += dist * sizeof(type); \
}

#define MEM_VECTOR_FORWARD_LIST_T_HELPER(type, unique_str) \
struct type##_forward_list_##unique_str { \
  type val; \
  type *next; \
}; \
static void* type##_next(void *ptr) { \
  return ((struct type##_forward_list_##unique_str *)ptr)->next; \
}

#define MEM_VECTOR_FORWARD_LIST_T(type) \
 MEM_VECTOR_FORWARD_LIST_T_HELPER(type, LOR_UNIQUE_COMPILER_STRING)

#endif


