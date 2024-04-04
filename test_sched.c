#include <stdio.h>

#include "g.h"

void f1(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f1()");
    goyield1();
  }
}

void f2(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f2()");
    goyield1();
  }
}

int main(void) {
  goinit();
  go(f1, 0);
  go(f2, 0);
  gowait();
}
