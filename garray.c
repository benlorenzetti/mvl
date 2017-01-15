// garray.c

#include <stdio.h>
#include "piv_garray.h"

int main() {
  char arr[1000];
  piv_slice array1 = PIV_GARRAY;
  printf("array size = %d\n", (int) PIV_SIZE(array1));
  printf("array capacity = %d\n", (int) PIV_CAPACITY(array1));

  piv_garray.sbrk(&array1.state, 100);
  printf("array size = %d\n", (int) PIV_SIZE(array1));
  printf("array capacity = %d\n", (int) PIV_CAPACITY(array1));
}

