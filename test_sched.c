#include <stdio.h>

#include "gg.h"

void f1(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f1()");
    gg_yield1();
  }
}

void f2(void) {
  int i;
  for (i = 0; i < 4; i++) {
    puts("f2()");
    gg_yield1();
  }
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg(f2, 0);
  gg_wait();
}
