#ifndef LOR_VECTOR_H
#define LOR_VECTOR_H

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
 *  This header file is a part of a generic container library for programming
 *  with object-oriented style in C. Lor vector implements a dynamic array
 *  for storing generic types and user-defined structs.
 *
 *  A vector is automatically resized as needed by the lor_vector_ functions
 *  provided for adding, removing, swapping, etc.
 *  Stuctures of any size may be stored and a copy constructor and
 *  destructor can be registered with the container to support deep copying.
 *
 *  The functionality is designed to be similar to C++'s std::vector class.
 *  In C++, classes mix the ideas of function namespace and data structures.
 *  For object-oriented programming style in C, you can mix these manually.
 *  For example,
 *                        complex1.add(complex2);
 *
 *  in C++, could be implemented equivalently as:
 *
 *                  complex.add(&complex1, complex2);
 *  in C.
 */

/*  The following return codes are defined for lor_vector_ functions.
 *  If you write your own copy constructor functions, they should
 *  return LOR_VECTOR_EXIT_SUCCESS or an error code of your own choosing
 *  outside the range of the library error codes defined here. 
 */

#define LOR_VECTOR_EXIT_SUCCESS 0
#define LOR_VECTOR_ALLOCATION_FAILURE 1

/*  The type "lor_vetlor_copy_f" is defined as a "pointer to a function with a
 *  generic destination and source pointer parameters and integer return value".
 *  Functions cast to this type can be registered as the copy constructor for
 *  the user-defined struct being stored.
 *  Likewise there is a type for a generic destructor that can be registered
 *  with a Lor vector.
 */

typedef void (*lor_vector_copy_f) (void*, const void*); 
typedef void (*lor_vector_dest_f) (void*);

/*  The template type for the container is stored in the lor_vector_s metadata.
 */

enum lor_vector_data_type
{
  CHAR,
  SHORT,
  INT,
  LONG,
  FLOAT,
  DOUBLE,
  PTR,
  USER_DEFINED_STRUCT
};

/*  The structure lor_vector_s is used as a handle for the vector and has size
 *  metadata and pointers to the dynamic array and template type functions.
 *  When declaring a vector, the metadata should always be initialized with the
 *  LOR_VECTOR macro to declare the template type and constructor/destructor
 *  functions. An example declaration for a vector holding integers and using
 *  the default copy constructor/destructor is:
 *
 *     lor_vector_s prime_numbers = LOR_INIT(int, NULL, NULL);
 */

struct lor_vector_s
{
  enum lor_vector_data_type d_type;
  const int t_size;
  lor_vector_copy_f copy_constructor;
  lor_vector_dest_f destructor;
  int alloc_len; // note: negative alloc_len indicates an explicit reservation
  void* begin;   // that should not be shrunk by the auto_reserve() function
  void* end;
};

#define LOR_VECTOR(T_type, constructor, destructor)                   \
{                                                                     \
  (!strcmp(#T_type,"char")?CHAR:   (!strcmp(#T_type,"short")?SHORT:   \
  (!strcmp(#T_type,"int")?INT:     (!strcmp(#T_type,"long")?LONG:     \
  (!strcmp(#T_type,"float")?FLOAT: (!strcmp(#T_type,"double")?DOUBLE: \
  (strchr(#T_type,'*')?PTR:        USER_DEFINED_STRUCT))))))),        \
  (sizeof(T_type)),                                                   \
  ((void (*)(void*, const void*))(constructor)),                      \
  ((void (*)(void*))(destructor)),                                    \
  0,                                                                  \
  NULL,                                                               \
  NULL                                                                \
}

/*  Constants A and B define the reservation policy for the automatic memory
 *  managenment. When the current reserved space x is insufficient, the array
 *  is reallocated to a new memory block of size
 *
 *                           x' = A*x + B
 *
 *  Reserved space will also shrink according to: x' = (x - B) / A. 
 *  Valid ranges for A and B are:
 *     {A e Real: A >= 1}  (to ensure growth)
 *     {B e Natural: B >= 1}  (to ensure growth for small x)
*/

#define LOR_VECTOR_A 1.3
#define LOR_VECTOR_B 1

/******************************_Vector Functions_******************************/

void* lor_vector_auto_reserve (struct lor_vector_s *);
void* lor_vector_at (const struct lor_vector_s*, int);

int lor_vector_push_back (struct lor_vector_s*, ...);
/*  The push back function copies a new element to the end of the array using
 *  memcpy() or the user defined copy constructor.
 *
 *  param1:  pointer to the lor_vector to which the element will be added
 *  param2:  the element to be copied onto the back of the array.
 *           (This parameter is variadic depending on the sizeof each element.
 *            If t_size matches a defined type (char, int, long, etc), then it
 *            should be passed by value, otherwise larger types should be passed
 *            like a const pointer. I.e. think of this function as either
 *                int lor_vector_push_back(struct lor_vector_s*, int);
 *            or
 *                int lor_vector_push_back(struct lor_vector_s*, const void*);
 *            if the type is a structure different than a predefined C type.)
 *  return:  LOR_EXIT_SUCCESS (0), or a LOR_VECTOR_ failure code, or a user
 *           defined code from the copy contstructor if it did not return
 *           LOR_EXIT_SUCCESS.
*/

int lor_vector_insert (struct lor_vector_s*, int, ...);
int lor_vector_size (const struct lor_vector_s*);

/***************************_Library Namespace Directives_********************/
/*
 *  To avoid namespace conflicts with outside code, all functions, structures,
 *  and typedefs declared in this library are prefixed by "lor_vector_" and 
 *  all macros by "LOR_VECTOR_". Functions can be called using their full
 *  prefix name or through a namespace struct provided for shortening code.
 *  The default namespace is "lor_vector" but this can be changed by defining
 *  LOR_VECTOR_NAMESPACE in your code.
 *  
 *  For example, the push back function could be called the following ways:
 *
 *    1.   lor_vector_push_back();
 *
 *    2.   lor_vector.push_back();
 *
 *    3.   #define LOR_VECTOR_NAMESPACE vec
 *         vec.push_back();
 */

#define LOR_VECTOR_CONCAT(x, y) x ## y
#define LOR_VECTOR_TYPENAME_MACRO(new_name) LOR_VECTOR_CONCAT(new_name, _t)

#define LOR_VECTOR_FUNCTION_NAMESPACE lor_vector
#define LOR_VECTOR_TYPENAME lor_vector_t
#ifdef LOR_VECTOR_NAMESPACE
  #undef  LOR_VECTOR_FUNCTION_NAMESPACE
  #define LOR_VECTOR_FUNCTION_NAMESPACE  LOR_VECTOR_NAMESPACE
  #undef  LOR_VECTOR_TYPENAME
  #define LOR_VECTOR_TYPENAME LOR_VECTOR_TYPENAME_MACRO(LOR_VECTOR_NAMESPACE)
#endif

typedef struct lor_vector_s LOR_VECTOR_TYPENAME;

struct lor_vector_namespace_s
{
  void* (*const auto_reserve)(struct lor_vector_s*);
  void* (*const at)(const struct lor_vector_s*, int);
  int (*const push_back)(struct lor_vector_s*, ...);
  int (*const insert)(struct lor_vector_s*, int, ...);
  int (*const size)(const struct lor_vector_s*);
};

struct lor_vector_namespace_s const LOR_VECTOR_FUNCTION_NAMESPACE =
{
  lor_vector_auto_reserve,
  lor_vector_at,
  lor_vector_push_back,
  lor_vector_insert,
  lor_vector_size
};

extern struct lor_vector_namespace_s const LOR_VECTOR_FUNCTION_NAMESPACE;


/************************_Start of Implementation File_***********************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int lor_vector_size (const struct lor_vector_s* vec) {
  return (vec->end - vec->begin) / vec->t_size;
}

int lor_vector_insert (struct lor_vector_s* vec, int pos, ...) {
  // Ensure there is allocated space available for the new array
  if (!lor_vector_auto_reserve(vec))
    return LOR_VECTOR_ALLOCATION_FAILURE;

  // Shift rightwards all elements starting at pos
  if (vec->copy_constructor)
  {
    char* ptr;
    for (ptr=vec->end; ptr >= (char*)vec->begin + pos*vec->t_size; ptr -= vec->t_size)
      (*vec->copy_constructor) (ptr, ptr - vec->t_size);
  }
  else // rightward shift if (no copy constructor specified)
  {
    char* pos_ptr = vec->begin + pos * vec->t_size;
    int copy_byte_length = vec->t_size * (lor_vector_size(vec) - pos);
    memmove (pos_ptr + vec->t_size, pos_ptr, copy_byte_length);
  }

  // Enable access to the variadic parameter
  va_list args;
  va_start(args, pos);

  // Copy the 3rd parameter into the insert position
  char* insert_pos = vec->begin + pos * vec->t_size;
  /* If the template type is a machine type, then the user should have passed
     it by value and can be directly assigned. If a user-defined type, then
     the 2nd parameter is a generic pointer to memory and there may be a copy
     constructor registered */
  switch(vec->d_type)
  {
    case CHAR:;
      *((char*)insert_pos) = (char) va_arg(args, int);
    break;
    case SHORT:;
      *((short*)insert_pos) = (short) va_arg(args, int);
    break;
    case INT:;
      *((int*)insert_pos) = va_arg(args, int);
    break;
    case LONG:;
      *((long*)insert_pos) = va_arg(args, long);
    break;
    case FLOAT:;
      *((float*)insert_pos) = (float) va_arg(args, double);
    break;
    case DOUBLE:;
      *((double*)insert_pos) = va_arg(args, double);
    break;
    case PTR:;
      *((void**)insert_pos) = va_arg(args, void*);
    break;
    default:; // A user-defined type
      const void* ptr_pass = va_arg(args, const void*);
      if(!vec->copy_constructor)
        memmove(insert_pos, ptr_pass, vec->t_size);
      else
        (*vec->copy_constructor) (insert_pos, ptr_pass);
  }

  // End traversal of variadic function arguements
  va_end(args);

  // Increment the end pointer and return
  vec->end += vec->t_size;
  return LOR_VECTOR_EXIT_SUCCESS;

}// lor_vector_insert() END

int lor_vector_push_back (struct lor_vector_s* vec, ...) {
  // Ensure there is allocated space available for the new value
  if (!lor_vector_auto_reserve(vec))
    return LOR_VECTOR_ALLOCATION_FAILURE;
  // Enable access to variadic function arguements
  va_list args;
  va_start(args, vec);
  /* If the template type is a machine type, then the user should have passed
     it by value and can be directly assigned. If a user-defined type, then
     the 2nd parameter is a generic pointer to memory and there may be a copy
     constructor registered */
  switch(vec->d_type)
  {
    case CHAR:;
      *((char*)vec->end) = (char) va_arg(args, int);
    break;
    case SHORT:;
      *((short*)vec->end) = (short) va_arg(args, int);
    break;
    case INT:;
      *((int*)vec->end) = va_arg(args, int);
    break;
    case LONG:;
      *((long*)vec->end) = va_arg(args, long);
    break;
    case FLOAT:;
      *((float*)vec->end) = (float) va_arg(args, double);
    break;
    case DOUBLE:;
      *((double*)vec->end) = va_arg(args, double);
    break;
    case PTR:;
      *((void**)vec->end) = va_arg(args, void*);
    break;
    default:; // A user-defined type
      const void* ptr_pass = va_arg(args, const void*);
      if(!vec->copy_constructor)
        memmove(vec->end, ptr_pass, vec->t_size);
      else
        (*vec->copy_constructor) (vec->end, ptr_pass);
  }

  // End traversal of variadic function arguements
  va_end(args);

  // Increment the end pointer and return
  vec->end += vec->t_size;
  return LOR_VECTOR_EXIT_SUCCESS;
}// lor_vector_push_back() END

void* lor_vector_at (const struct lor_vector_s* vec, int n) {
  if (n >= (vec->end - vec->begin) / vec->t_size)
    return NULL; // out of bounds error
  return vec->begin + n * vec->t_size;
}

void* lor_vector_auto_reserve (struct lor_vector_s* dest) {
  // By default, assume that no resizing or relocation will be done-----------|
  int new_alloc_len = dest->alloc_len;
  char* new_alloc_ptr = dest->begin;
  int array_len = (dest->end - dest->begin) / dest->t_size;
  // Test if more memory needs to be reserved---------------------------------|
  if (array_len == abs(dest->alloc_len))  // note that negative alloc_size
  {                          // indicates it should not be automatically shrunk
    new_alloc_len = LOR_VECTOR_A * abs(dest->alloc_len) + LOR_VECTOR_B;
    new_alloc_ptr = realloc(dest->begin, new_alloc_len * dest->t_size);
    if (!new_alloc_ptr) // realloc() failure does not disturb original memory
      return NULL;// so return NULL but dont overwrite dest->begin/alloc_size 
  }
  // Test if memory should be released----------------------------------------|
  // It will not be if alloc_size is negative due to an explicit reservation
  else if (array_len < (dest->alloc_len - LOR_VECTOR_B) / LOR_VECTOR_A)
  {
    new_alloc_len = (dest->alloc_len - LOR_VECTOR_B) / LOR_VECTOR_A;
    new_alloc_ptr = realloc(dest->begin, new_alloc_len * dest->t_size);
    if (!new_alloc_ptr)
      return dest->begin; // failing to shrink the array isnt really a failure
  }
  // Otherwise the current reservation size is ok so no changes---------------|
  else
  {}
  // Save changes and return pointer to the current location of the data------|
  dest->alloc_len = new_alloc_len;
  dest->begin = new_alloc_ptr;
  dest->end = new_alloc_ptr + array_len * dest->t_size;
  return dest->begin;
}

#endif
