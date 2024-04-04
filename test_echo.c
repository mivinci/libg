#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "g.h"

void f1(void) {
  char buf[64];
  int fd, n;

  fd = STDIN_FILENO;
  setnonblock(fd);

  for (;;) {
    n = read(fd, buf, 64);
    if (n < 0) {
      printf("f1: read failed with %d", n);
      exit(1);
    }
    puts(buf);
  }
}

int main(void) {
  goinit();
  go(f1, 0);
  gowait();
}
