#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

#include "runtime.h"

ssize_t (*sys_read)(int, char *, size_t);
ssize_t (*sys_write)(int, const char *, size_t);
int (*sys_socket)(int, int, int);
int (*sys_accept)(int, struct sockaddr *, socklen_t *);

void hookinit(void) {
  sys_read = dlsym(RTLD_NEXT, "read");
  sys_write = dlsym(RTLD_NEXT, "write");
  sys_socket = dlsym(RTLD_NEXT, "socket");
  sys_accept = dlsym(RTLD_NEXT, "accept");
}

ssize_t read(int fd, char *buf, size_t n) {
  int m;
  while ((m = sys_read(fd, buf, n)) < 0 && errno == EAGAIN)
    await(fd, EV_READ, -1, 0);
  return m;
}

ssize_t write(int fd, char *buf, size_t n) {
  int m;
  while ((m = sys_write(fd, buf, n)) < 0 && errno == EAGAIN)
    await(fd, EV_WRITE, -1, 0);
  return m;
}

int accept(int fd, struct sockaddr *sa, socklen_t *n) {
  int peerfd;
  while ((peerfd = sys_accept(fd, sa, n)) < 0 && errno == EAGAIN)
    await(fd, EV_READ, -1, 0);
  setnonblock(peerfd);
  return peerfd;
}

int socket(int domain, int type, int protocol) {
  int fd;
  if ((fd = sys_socket(domain, type, protocol)) > 0)
    setnonblock(fd);
  return fd;
}
