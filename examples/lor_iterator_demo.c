#include <stdio.h>  // for printf()
#include <stdlib.h> // for atoi() and exit()
#include <time.h>   // for clock() and clock_t

#define LOR_VECTOR_NAMESPACE vec
#include "lor_vector.h"
#include "lor_iterator.h"

LOR_ITERATOR_DEFINE_RANDOM_ACCESS_TYPE(int);

int main(int argc, char** argv) {
  // Get desired size for the test vector
  if(argc != 2){
    printf("Usage: %s <test vector size (a natural number)>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int test_size = atoi(argv[1]);

  // Build a test vector of integers
  vec_t natural_numbers = LOR_VECTOR(int, NULL, NULL);
  int i;
  for(i = 1; i <= test_size; i++)
    vec.push_back(&natural_numbers, i);

  // Iterate through vector using normal pointer arithmetic
  clock_t ptr_time = clock();
  int *ptr = natural_numbers.begin;
  while(*ptr != test_size)
    ptr++;
  ptr_time = clock() - ptr_time;
  printf("Linear search time scanning through a %d integer vector", test_size);
  printf(" using int pointer: %d\n", (int)ptr_time);

  // Iterate through vector using lor_iterators
  clock_t iter_time = clock();
  LOR_ITERATOR_RANDOM_ACCESS_ITERATOR(iter, int) = LOR_ITERATOR_RANDOM_ACCESS(int);
  iter.ptr = natural_numbers.begin;
  while(*iter.ptr != test_size)
    lor_iterator_next(iter);
  iter_time = clock() - iter_time;
  printf("Linear search time scanning through a %d integer vector", test_size);
  printf(" using lor iterator: %d\n", (int)iter_time);

  return 0;
}
