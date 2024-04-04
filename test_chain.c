#include <stdio.h>
#include "gg.h"

void f3(void) {
  puts("f3()");
}

void f2(void) {
  puts("f2()");
  gg(f3, 0);
}

void f1(void) {
  puts("f1()");
  gg(f2, 0);
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg_wait();
}
