#include <stdio.h>

#include "g.h"

void f1(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f1()");
    asleep(2000);
  }
}

void f2(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f2()");
    asleep(2000);
  }
}

int main(void) {
  goinit();
  go(f1, 0);
  go(f2, 0);
  gowait();
}
