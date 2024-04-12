#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "gg.h"
#include "runtime.h"

void gg_init(void) {
  schedinit();
  netpollinit();
  hookinit();
}

void gg_wait(void) { schedule(); }

void gg_spawn(void (*f)(void *), void *arg, int size) { spawn(f, arg, size); }

void gg_yield(void) { yield1(); }

void gg_yield1(void) { yield(); }

void gg_sleep(long ms) { asleep(ms); }

struct gg_chan *gg_makechansize(int cap, int elsize) {
  return (struct gg_chan *)makechan(cap, elsize);
}

bool gg_send(struct gg_chan *cp, void *p) {
  return chansend((Chan *)cp, p, true);
}

bool gg_recv(struct gg_chan *cp, void *p) {
  return chanrecv((Chan *)cp, p, true);
}

void gg_close(struct gg_chan *cp) { return chanclose((Chan *)cp); }

int gg_dial(int net, const char *host, unsigned short port) {
  switch (net) {
  case GG_UDP:
    return dial0(SOCK_DGRAM, host, port);
  case GG_TCP:
    return dial0(SOCK_STREAM, host, port);
  default:
    throw("gg_dial: unsupported net %d", net);
  }
  __builtin_unreachable();
}

int gg_listen(int net, const char *host, unsigned short port) {
  switch (net) {
  case GG_UDP:
    return listen0(SOCK_DGRAM, host, port);
  case GG_TCP:
    return listen0(SOCK_STREAM, host, port);
  default:
    throw("gg_listen: unsupported net %d", net);
  }
  __builtin_unreachable();
}

int gg_dialunix(int net, const char *path) {
  switch (net) {
  case GG_UDP:
    return dialunix0(SOCK_DGRAM, path);
  case GG_TCP:
    return dialunix0(SOCK_STREAM, path);
  default:
    throw("gg_dialunix: unsupported net %d", net);
  }
  __builtin_unreachable();
}

int gg_listenunix(int net, const char *path) {
  switch (net) {
  case GG_UDP:
    return listenunix0(SOCK_DGRAM, path);
  case GG_TCP:
    return listenunix0(SOCK_STREAM, path);
  default:
    throw("gg_listenunix: unsupported net %d", net);
  }
  __builtin_unreachable();
}
