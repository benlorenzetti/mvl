// pique.c for testing pique

#include <stdio.h>
#include "pique.h"

#ifndef PIQUE_INT_XT
  #define PIQUE_INT_XT
  PIQUE_DEFINE_XT(int)
#endif


int main() {
  PIQUE_XT(int) queue1 = PIQUE(int);
  printf("empty ? %s\n", PIV_EMPTY(queue1.pie) ? "yes" : "no");
  for(int i=1; i<=100; i++)
    queue1.push(&queue1, i);

  printf("array size = %d\n", (int) PIV_SIZE(queue1.pie));
  printf("array capacity = %d\n", (int) PIV_CAPACITY(queue1.pie));

  PIV_VECTOR_XT(int) iter = PIV_LVEC(queue1.pie);
  do {
    PIV_INC(iter);
    printf("%d ", *iter.end);
  } while (iter.end != queue1.pie.end);
 
}

