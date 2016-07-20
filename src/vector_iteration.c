#include <stdio.h>  // for printf()
#include <stdlib.h> // for atoi() and exit()
#include <time.h>   // for clock() and clock_t

#include "mem_slice.h"

MEM_VECTOR_RANDOM_ACCESS_T(int);

int main(int argc, char** argv) {
  // Get desired size for the test vector
  if(argc != 2){
    printf("Usage: %s <test vector size (a natural number)>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int test_size = atoi(argv[1]);

  // Build a dynamic test array of integers
  int* natural_numbers = malloc(test_size * sizeof(int));
  if(natural_numbers == NULL)
    printf("malloc() failure.\n"), exit(EXIT_FAILURE);
  int i;
  for(i = 1; i <= test_size; i++)
    natural_numbers[i] = i;

  // Iterate through array using normal pointer arithmetic
  clock_t ptr_time = clock();
  int *ptr = natural_numbers;
  while(*ptr != test_size)
    ptr++;
  ptr_time = clock() - ptr_time;
  printf("Linear search time scanning through a %d integer array", test_size);
  printf(" using int pointer: %d\n", (int)ptr_time);

  // Iterate through array using a memory vector
  clock_t iter_time = clock();
  mem_vector_T(int) iter = {{NULL, int_array_inc, int_array_dec, int_add}};
  iter.ptr = natural_numbers;
  while(*iter.ptr != test_size)
    mem_next(iter);
  iter_time = clock() - iter_time;
  printf("Linear search time scanning through a %d integer array", test_size);
  printf(" using mem vector: %d\n", (int)iter_time);

  return 0;
}
