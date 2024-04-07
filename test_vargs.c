#include "gg.h"
#include <stdarg.h>
#include <stdio.h>

void f2(void *p, ...) {
  char buf[64];
  va_list ap;
  va_start(ap, p);
  /*
    FIXME:
    REASON: EXC_BAD_ACCESS
    INSTRUCTION: movdqa %xmm0, -180(%rbp)

    macOS requires the stack to be 16 bytes aligned,
    but somehow it is not when vsprintf is being called
   */
  vsprintf(buf, "%d %d %d\n", ap);
  puts(buf);
}

void f1(void) {
  f2(0, 0x1000, 0x2000, 0x3000);
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg_wait();
}