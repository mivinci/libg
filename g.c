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
  spawn(f, arg, 4096);
}

void goyield(void) {
  yield();
}

void goyield1(void) {
  yield1();
}

void gosleep(long ms) {
  asleep(ms);
}
