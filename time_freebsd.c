#include <time.h>

#include "runtime.h"

long timens(void) {
  struct timespec ts;
  int n;
  n = clock_gettime(CLOCK_REALTIME, &ts);
  if (n < 0)
    throw("timens: clock_gettime failed with %d", n);
  return ts.tv_sec * 1000000000L + ts.tv_nsec;
}
