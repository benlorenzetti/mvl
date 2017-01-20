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
  printf("det ? %s\n", PIV_DET(pie1.slice.rvec) ? "yes" : "no");
  for(int i=1; i<=100; i++)
    pie1.push(&pie1, i);

  printf("array size = %lu\n", PIV_SIZE(pie1.slice));
  printf("array capacity = %lu\n", PIV_CAPACITY(pie1.slice));

  PIVEC_XT(int) iter = PIVEC(int);
  if (PIV_DET(iter.lvec = pie1.slice.state.lvec))
  do {PIV_INC(iter);
    printf("%d ", *iter.end);
  } while (iter.end != pie1.slice.end);
 
}

