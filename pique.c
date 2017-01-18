// pique.c for testing pique

#include <stdio.h>
#include "pique.h"

#ifndef PIQUE_INT_XT
  #define PIQUE_INT_XT
  PIQUE_DEFINE_XT(int)
#endif


int main() {
  PIQUE_XT(int) pie1 = PIQUE(int);
  printf("empty ? %s\n", PIV_EMPTY(pie1.slice) ? "yes" : "no");
  for(int i=1; i<=100; i++)
    pie1.push(&pie1, i);

  printf("array size = %d\n", (int) PIV_SIZE(pie1.slice));
  printf("array capacity = %d\n", (int) PIV_CAPACITY(pie1.slice));

  PIV_VECTOR_XT(int) iter = PIV_LVEC(pie1.slice);
  do {
    PIV_INC(iter);
    printf("%d ", *iter.end);
  } while (iter.end != pie1.slice.end);
 
}

