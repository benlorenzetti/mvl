#include <stdio.h>

int main() {
  int begin = 0;
  int end = 100;
  int i;
  for(i = begin; i < end; i++) {}

  printf("after loop i is %s end\n", (i == end)?"equal to":"not equal to");
  printf("begin = %d, end = %d, i = %d\n", begin, end, i);
  return 0;
}
