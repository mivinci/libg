#include "gg.h"
#include <stdarg.h>
#include <stdio.h>

void f2(void *p, ...) {
  va_list ap;
  va_start(ap, p);
  vfprintf(stdout, "%d %d %d", ap);
  va_end(ap);
}

void f1(void) {
  f2(0, 100, 200, 300);
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg_wait();
}