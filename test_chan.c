#include "gg.h"
#include <stdio.h>
#include <stdlib.h>

struct gg_chan *cp;

void f3(void) {
  int i;
  for (;;) {
    if (!gg_recv(cp, &i)) {
      printf("f3 failed receiving\n");
      exit(1);
    }
    printf("f3 recv %d\n", i);
  }
}

void f2(void) {
  int i;
  for (;;) {
    if (!gg_recv(cp, &i)) {
      printf("f2 failed receiving\n");
      exit(1);
    }
    printf("f2 recv %d\n", i);
  }
}

void f1(void) {
  int i;
  for (i = 0; i < 8; i++) {
    if (!gg_send(cp, &i)) {
      printf("f1 failed sending %d\n", i);
      exit(1);
    }
    printf("f1 send %d\n", i);
    gg_sleep(1000);
  }
}

int main(void) {
  gg_init();
  cp = gg_makechan(int, 0); /* un-buffered channel */
  gg(f1, 0);
  gg(f2, 0);
  gg(f3, 0);
  gg_wait();
  return 0;
}
