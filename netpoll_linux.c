#include <errno.h>
#include <sys/epoll.h>

#include "runtime.h"

static int epfd = -1;

void netpollinit(void) {
  epfd = epoll_create(1);
  if (epfd < 0)
    throw("netpollinit: epoll_create failed with errno %d", errno);
  setcloexec(epfd);
}

int netpollopen(Event *ep) {
  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
  ev.data.ptr = (void *)ep;
  return epoll_ctl(epfd, EPOLL_CTL_ADD, ep->fd, &ev);
}

G *netpoll(long ns) {
  struct epoll_event ev[64], *ep;
  G *gp;
  int n, i, mode, ms = -1;

  if (epfd < 0)
    return nil;

  if (ns > 0)
    ms = ns / 1000000L;

retry:
  gp = nil;
  n = epoll_wait(epfd, ev, len(ev), ms);
  if (n < 0) {
    if (n == -EINTR)
      goto retry;
    throw("netpoll: epoll_wait failed with %d", n);
  }
  for (i = 0; i < n; i++) {
    ep = ev + i;
    mode = 0;
    if (ep->events & (EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR))
      mode |= EV_READ;
    if (ep->events & (EPOLLOUT | EPOLLHUP | EPOLLERR))
      mode |= EV_WRITE;
    if (mode)
      netpollready(&gp, (Event *)ep->data.ptr, mode);
  }
  return gp;
}
