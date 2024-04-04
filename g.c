#include "g.h"
#include "runtime.h"

void goinit(void) {
  schedinit();
  netpollinit();
  hookinit();
}

void gowait(void) {
  schedule();
}

void gospawn(void (*f)(void *), void *arg) {
  spawn((void (*)(void *))f, arg, 4096);
}
