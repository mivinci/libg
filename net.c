#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include "runtime.h"

static int bind0(int, const char *, unsigned short);
static int bindunix(int, const char *);
static int connect0(int, const char *, unsigned short);
static int connectunix(int, const char *);

void setcloexec(int fd) {}

void setnonblock(int fd) {
  int n;
  n = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  if (n < 0)
    throw("setnonblock: fcntl failed with errno %d", errno);
}

int listen0(int type, const char *host, unsigned short port) {
  int fd, n;

  fd = bind0(type, host, port);
  if (fd < 0)
    throw("listen0: bind0 failed with %d", fd);
  if (type != SOCK_STREAM)
    return fd;
  n = listen(fd, 64);
  if (n < 0)
    throw("listen0: listen failed with errno %d", errno);
  return fd;
}

int dial0(int type, const char *host, unsigned short port) {
  int fd;

  fd = connect0(type, host, port);
  if (fd < 0)
    throw("dial0: connect0 failed with errno %d", errno);
  return fd;
}

int listenunix0(int type, const char *path) {
  int fd, n;

  fd = bindunix(type, path);
  if (fd < 0)
    throw("listenunix0: bind0 failed with %d", fd);
  if (type != SOCK_STREAM)
    return fd;
  n = listen(fd, 64);
  if (n < 0)
    throw("listenunix0: listen failed with errno %d", errno);
  return fd;
}

int dialunix0(int net, const char *path) {
  int fd;

  fd = connectunix(SOCK_STREAM, path);
  if (fd < 0)
    throw("dialunix0: connectunix failed with errno %d", errno);
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

static int bindunix(int type, const char *path) {
  struct sockaddr_un sa;
  int fd, n;

  memset(&sa, 0, sizeof sa);
  strncpy(sa.sun_path, path, sizeof(sa.sun_path) - 1);
  sa.sun_family = AF_UNIX;

  fd = socket(sa.sun_family, type, 0);
  if (fd < 0)
    throw("bindunix: socket failed with errno %d", errno);

  n = bind(fd, (struct sockaddr *)&sa, sizeof sa);
  if (n < 0)
    throw("bindunix: bind failed with errno %d", errno);

  return fd;
}

static int connectunix(int type, const char *path) {
  struct sockaddr_un sa;
  int fd, n;

  memset(&sa, 0, sizeof sa);
  strncpy(sa.sun_path, path, sizeof(sa.sun_path) - 1);
  sa.sun_family = AF_UNIX;

  fd = socket(sa.sun_family, type, 0);
  if (fd < 0)
    throw("bindunix: socket failed with errno %d", errno);

  n = connect(fd, (struct sockaddr *)&sa, sizeof sa);
  if (n < 0)
    throw("bindunix: connect failed with errno %d", errno);

  return fd;
}
