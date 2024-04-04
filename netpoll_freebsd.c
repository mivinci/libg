#include "runtime.h"
#include <errno.h>
#include <sys/event.h>
#include <time.h>


static int kq = -1;

void netpollinit(void) {
  kq = kqueue();
  if (kq < 0)
    throw("netpollinit: kqueue failed with %d", kq);
  setcloexec(kq);
}

int netpollopen(Event *ep) {
  struct kevent ev[2];
  ev[0].ident = ep->fd;
  ev[0].filter = EVFILT_READ;
  ev[0].flags = EV_ADD | EV_CLEAR; /* edge-triggered */
  ev[0].fflags = 0;
  ev[0].data = 0;
  ev[0].udata = (void *)ep;
  ev[1] = ev[0];
  ev[1].filter = EVFILT_WRITE;
  return kevent(kq, ev, 2, nil, 0, nil);
}

G *netpoll(long ns) {
  struct kevent ev[64], *ep;
  struct timespec ts, *tsp = nil;
  G *gp;
  int n, i, mode;

  if (kq < 0)
    return nil;

  if (ns >= 0) {
    ts.tv_sec = ns / 1000000000L;
    ts.tv_nsec = ns % 1000000000L;
    tsp = &ts;
  }

retry:
  gp = nil;
  n = kevent(kq, nil, 0, ev, len(ev), tsp);
  if (n < 0) {
    if (n == -EINTR)
      goto retry;
    throw("netpoll: kevent failed with %d", n);
  }
  for (i = 0; i < n; i++) {
    ep = ev + i;
    mode = 0;
    if (ep->filter == EVFILT_READ)
      mode |= EV_READ;
    if (ep->filter == EVFILT_WRITE)
      mode |= EV_WRITE;
    if (mode)
      netpollready(&gp, (Event *)ep->udata, mode);
  }
  return gp;
}
