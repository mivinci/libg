#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "gg.h"
#include "runtime.h"

static int bind0(int, const char *, unsigned short);
static int connect0(int, const char *, unsigned short);

void setcloexec(int fd) {}

void setnonblock(int fd) {
  int n;
  n = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  if (n < 0)
    throw("setnonblock: fcntl failed with errno %d", errno);
}

int listen0(int type, const char *host, unsigned short port) {
  int fd, n;
  switch (type) {
  case GG_UDP:
    fd = bind0(SOCK_DGRAM, host, port);
    break;
  case GG_TCP:
    fd = bind0(SOCK_STREAM, host, port);
    if (fd < 0)
      throw("listen0: bind0 failed with %d", fd);
    n = listen(fd, 5);
    if (n < 0)
      throw("listen0: listen failed with errno %d", errno);
    break;
  default:
    fd = -1;
    throw("listen0: unsupported type %d", type);
  }
  return fd;
}

int dial0(int type, const char *host, unsigned short port) {
  int fd;
  switch (type) {
  case GG_UDP:
    fd = connect0(SOCK_DGRAM, host, port);
    break;
  case GG_TCP:
    fd = connect0(SOCK_STREAM, host, port);
    break;
  default:
    fd = -1;
    throw("dial0: unsupported type %d", type);
  }
  return fd;
}

static int isv6(const char *p) {
  for (; *p; p++)
    if (*p == ':')
      return 1;
  return 0;
}

static int bind0(int type, const char *host, unsigned short port) {
  struct addrinfo hint, *ap;
  char b[6];
  int n, fd = -1;

  snprintf(b, 6, "%u", port);
  memset(&hint, 0, sizeof hint);
  hint.ai_family = isv6(host) ? AF_INET6 : AF_INET;
  hint.ai_socktype = type;
  hint.ai_flags = AI_PASSIVE;

  n = getaddrinfo(host, b, &hint, &ap);
  if (n < 0)
    throw("bind0: getaddrinfo failed with %d", n);

  for (; ap; ap = ap->ai_next) {
    fd = socket(ap->ai_family, ap->ai_socktype, ap->ai_protocol);
    if (fd < 0)
      continue;
    n = bind(fd, ap->ai_addr, ap->ai_addrlen);
    if (n < 0)
      continue;
    break;
  }
  freeaddrinfo(ap);
  return fd;
}

int connect0(int type, const char *host, unsigned short port) {
  struct addrinfo hint, *ap;
  char b[6];
  int n, fd;

  snprintf(b, 6, "%u", port);
  memset(&hint, 0, sizeof hint);
  hint.ai_family = isv6(host) ? AF_INET6 : AF_INET;
  hint.ai_socktype = type;
  hint.ai_flags = AI_PASSIVE;

  n = getaddrinfo(host, b, &hint, &ap);
  if (n < 0)
    throw("connect0: getaddrinfo failed with %d", n);

  fd = socket(hint.ai_family, type, 0);
  if (fd < 0)
    throw("connect0: socket failed with errno %d", errno);

  for (; ap; ap = ap->ai_next) {
    n = connect(fd, ap->ai_addr, ap->ai_addrlen);
    if (n < 0)
      continue;
    break;
  }
  freeaddrinfo(ap);
  return fd;
}
