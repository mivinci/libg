#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "runtime.h"

static int bind0(int, const char *, unsigned short);
static int dial0(int, const char *, unsigned short);

void setcloexec(int fd) {}

void setnonblock(int fd) {
  int n;
  n = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  if (n < 0)
    throw("setnonblock: fcntl failed with errno %d", errno);
}

int listentcp(const char *host, unsigned short port) {
  int fd, n;
  fd = bind0(SOCK_STREAM, host, port);
  if (fd < 0)
    throw("listentcp: bind0 failed with %d", fd);
  n = listen(fd, 5);
  if (n < 0)
    throw("listentcp: listen failed with errno %d", errno);
  return fd;
}

int dialtcp(const char *host, unsigned short port) {
  return dial0(SOCK_STREAM, host, port);
}

int listenudp(const char *host, unsigned short port) {
  return bind0(SOCK_DGRAM, host, port);
}

int dialudp(const char *host, unsigned short port) {
  return dial0(SOCK_DGRAM, host, port);
}

static int isv6(const char *p) {
  for (; p; p++)
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

static int dial0(int type, const char *host, unsigned short port) {
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
    throw("dial0: getaddrinfo failed with %d", n);

  fd = socket(hint.ai_family, type, 0);
  if (fd < 0)
    throw("dial0: socket failed with errno %d", errno);

  for (; ap; ap = ap->ai_next) {
    n = connect(fd, ap->ai_addr, ap->ai_addrlen);
    if (n < 0)
      continue;
    break;
  }
  freeaddrinfo(ap);
  return fd;
}
