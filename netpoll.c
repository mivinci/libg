#include "runtime.h"

void netpollready(G **gpp, Event *ep, int mode) {
  if (ep->fd < 0) /* expired */
    return;
  if (ep->mode & mode) {
    ep->g->next = *gpp;
    *gpp = ep->g;
  }
}
