#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "gg.h"

void f2(int fd) {
  char buf[64];
  int n;

  for (;;) {
    n = read(fd, buf, 64);
    if (n < 0) {
      printf("f2: read failed with errno %d", errno);
      break;
    }
    n = write(fd, buf, n);
    if (n < 0) {
      printf("f2: write failed with errno %d", errno);
      break;
    }
  }
}

void f1(void) {
  struct sockaddr sa;
  socklen_t n;
  int fd, peerfd;

  fd = gg_listen(GG_TCP, "localhost", 8000);

  for (;;) {
    peerfd = accept(fd, &sa, &n);
    if (peerfd < 0) {
      printf("f1: accept failed with errno %d", errno);
      break;
    }
    gg(f2, peerfd);
  }

  close(fd);
}

int main(void) {
  gg_init();
  gg(f1, 0);
  gg_wait();
}
