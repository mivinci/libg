#include <stdio.h>
#include "g.h"

void f3(void) {
  puts("f3()");
}

void f2(void) {
  puts("f2()");
  go(f3, 0);
}

void f1(void) {
  puts("f1()");
  go(f2, 0);
}

int main(void) {
  goinit();
  go(f1, 0);
  gowait();
}
