#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "gg.h"

void f2(void *arg) {
  char buf[64];
  int fd, n;

  fd = (int)arg;
  for (;;) {
    n = read(fd, buf, 64);
    if (n < 0) {
      printf("f2: read failed with errno %d", errno);
      exit(1);
    }
    write(fd, buf, n);
  }
}

int f1(void) {
  struct sockaddr sa;
  socklen_t n;
  int fd, peerfd;

  fd = gg_listen(GG_TCP, "localhost", 8000);

  for (;;) {
    peerfd = accept(fd, &sa, &n);
    gg(f2, (void *)peerfd);
  }
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg_wait();
}
