#include <stdlib.h>
#include <string.h>

#include "runtime.h"

static bool trysend(Chan *, void *);
static bool tryrecv(Chan *, void *);
static void push(WaitQ *, G *);
static G *pop(WaitQ *);

void *at(Chan *cp, int i) { return cp->ptr + i * cp->elsize; }

bool full(Chan *cp) {
  if (cp->cap == 0)
    return cp->recvq.head == nil;
  return cp->cap == cp->len;
}

bool empty(Chan *cp) {
  if (cp->cap == 0)
    return cp->sendq.head == nil;
  return cp->len == 0;
}

Chan *makechan(int cap, int elsize) {
  Chan *cp;
  if (!(cp = malloc(cap * elsize + sizeof *cp)))
    return nil;
  memset(cp, 0, sizeof *cp);
  cp->cap = cap;
  cp->elsize = elsize;
  cp->ptr = cp + 1;
  return cp;
}

bool chansend(Chan *cp, void *p, bool block) {
  G *gp;

  if (!cp)
    return false;
  if (trysend(cp, p))
    return true;
  if (!block)
    return false;
  gp = getg();
  gp->p = p;
  push(&cp->sendq, gp);
  yield();
  return true;
}

bool chanrecv(Chan *cp, void *p, bool block) {
  G *gp;

  if (!cp)
    return false;
  if (tryrecv(cp, p))
    return true;
  if (!block)
    return false;
  gp = getg();
  gp->p = p;
  push(&cp->recvq, gp);
  yield();
  return true;
}

void chanclose(Chan *cp) {
  G *gp;

  if (cp->closed)
    throw("chanclose: close of closed channel");
  cp->closed = true;

  for (;;) {
    gp = pop(&cp->recvq);
    if (!gp)
      break;
    ready(gp);
  }

  for (;;) {
    gp = pop(&cp->sendq);
    if (!gp)
      break;
    ready(gp);
  }
}

int selectgo(Case *cas, int n, bool block) {
  /* TODO */
  throw("selectgo: not implemented");
  return -1;
}

bool trysend(Chan *cp, void *p) {
  G *gp;
  void *q;

  if (cp->closed)
    throw("trysend: send on closed channel");
  if (full(cp))
    return false;

  gp = pop(&cp->recvq);
  if (gp) {
    memcpy(gp->p, p, cp->elsize);
    ready(gp);
    return true;
  }
  q = at(cp, cp->sendx);
  memcpy(q, p, cp->elsize);
  cp->sendx = (cp->sendx + 1) % cp->cap;
  cp->len++;
  return true;
}

bool tryrecv(Chan *cp, void *p) {
  G *gp;
  void *q;

  if (cp->closed || empty(cp))
    return false;

  gp = pop(&cp->sendq);
  if (gp) {
    memcpy(p, gp->p, cp->elsize);
    ready(gp);
    return true;
  }
  q = at(cp, cp->recvx);
  memcpy(p, q, cp->elsize);
  cp->recvx = (cp->recvx + 1) % cp->cap;
  cp->len--;
  return true;
}

static void push(WaitQ *q, G *gp) {
  if (q->tail)
    q->tail->next = gp;
  else
    q->head = gp;
  q->tail = gp;
}

static G *pop(WaitQ *q) {
  G *gp;
  gp = q->head;
  if (gp) {
    q->head = gp->next;
    gp->next = nil;
  }
  if (gp == q->tail)
    q->tail = nil;
  return gp;
}
