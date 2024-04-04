#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "gg.h"

void handle(void *arg) {
  char buf[64];
  int fd, n;

  fd = (int)arg;
  for (;;) {
    n = read(fd, buf, 64);
    if (n < 0) {
      printf("handle: read failed with errno %d", errno);
      exit(1);
    }
    write(fd, buf, n);
  }
}

int main(void) {
  struct sockaddr sa;
  socklen_t n;
  int fd, peerfd;

  fd = gg_listen(GG_TCP, "localhost", 8000);

  for (;;) {
    peerfd = accept(fd, &sa, &n);
    gg(handle, (void *)peerfd);
  }
}
