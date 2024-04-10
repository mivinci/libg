#include <stdio.h>
#include <stdarg.h>

void f(int n, ...) {
  int d = 400;
  int a, b, c;
  va_list ap;
  va_start(ap, n);
  a = va_arg(ap, int);
  b = va_arg(ap, int);
  c = va_arg(ap, int);
  printf("%d %d %d %d %d\n", n, a, b, c, d);
}


void g() {
  f(3, 100, 200, 300);
}