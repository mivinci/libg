#include "gg.h"
#include "runtime.h"


void gg_init(void) {
  schedinit();
  netpollinit();
  hookinit();
}

void gg_wait(void) {
  schedule();
}

void gg_spawn(void (*f)(void *), void *arg, int size) {
  spawn(f, arg, size);
}

void gg_yield(void) {
  yield();
}

void gg_yield1(void) {
  yield1();
}

void gg_sleep(long ms) {
  asleep(ms);
}

struct gg_chan *gg_makechansize(int cap, int elsize) {
  return (struct gg_chan*)makechan(cap, elsize);
}

bool gg_send(struct gg_chan *cp, void *p) {
  return chansend((Chan*)cp, p, true);
}

bool gg_recv(struct gg_chan *cp, void *p) {
  return chanrecv((Chan*)cp, p, true);
}

void gg_close(struct gg_chan *cp) {
  return chanclose((Chan*)cp);
}

int gg_dial(int type, const char *host, unsigned short port) {
  return dial0(type, host, port);
}

int gg_listen(int type, const char *host, unsigned short port) {
  return listen0(type, host, port);
}
