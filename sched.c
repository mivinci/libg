#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

static void exit0(void);
static G *gget(void);
static void gput(G *);
static G *gfget(void);
static void gfput(G *);
static void resume(G *);
extern int gosave(Gobuf *) asm("gosave");
extern int gogo(Gobuf *) asm("gogo");

static __thread Sched sched;

void debug(const char *fmt, ...) {
  va_list ap;

  fprintf(stdout,
          "\ndebug(%d):\n"
          "  sched.genid = %d\n"
          "  sched.glen  = %d\n",
          sched.g->id, sched.genid, sched.glen);
  va_start(ap, fmt);
  vfprintf(stdout, fmt, ap);
  va_end(ap);
  putc('\n', stdout);
}

void throw(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  putc('\n', stderr);
  exit(1);
}

void schedinit(void) {
  sched.gfree = nil;
  sched.ghead = nil;
  sched.gtail = nil;
  sched.genid = 0;
  sched.glen = 0;
  timersinit(&sched.t);
}

G *getg(void) { return sched.g; }

static void exit0() {
  G *gp;
  gp = sched.g;
  sched.g = nil;
  gp->state = G_DEAD;
  gfput(gp);
  schedule();
}

void spawn(void (*f)(void *), void *arg, int size) {
  G *gp;
  char *sp;
  void **ret;

  gp = gfget();
  if (!gp)
    gp = malloc(size + sizeof *gp);
  if (!gp)
    throw("spawn: out of memory");

  gp->stackguard = gp + 1;
  gp->state = G_IDLE;
  gp->next = nil;
  gp->mp = 0;
  gp->ev.fd = gp->ev.when = -1;
  gp->ev.g = gp;
  sp = (char *)gp->stackguard + size - sizeof(void *);
  sp = (char *)((unsigned long)sp & -16L);
  memset(gp->ctx.gr, 0, sizeof gp->ctx.gr);
  /*
    NOTE: macOS requires the stack to be 16 bytes allgned
    on 64-bit machines, so we have to leave 8 bytes for
    instruction `push %rbp` that happens at the beginning
    of each coroutine.
  */
  sp -= 8;
  ret = (void **)sp;
  *ret = (void *)exit0;
  gp->ctx.sp = (void *)sp;
  gp->ctx.pc = (void *)f;
  gp->ctx.gr[6] = arg; /* %rdi */
  gp->id = sched.genid++;

  ready(gp);
}

static G *gfget(void) {
  G *gp;
  gp = sched.gfree;
  if (gp)
    sched.gfree = gp->next;
  return gp;
}

static void gfput(G *gp) {
  if (!gp)
    return;
  gp->next = sched.gfree;
  sched.gfree = gp;
}

static G *gget(void) {
  G *gp;
  gp = sched.ghead;
  if (gp) {
    sched.ghead = gp->next;
    gp->next = nil;
  }
  if (gp == sched.gtail)
    sched.gtail = nil;
  if (gp)
    sched.glen--;
  return gp;
}

static void gput(G *gp) {
  if (!gp)
    return;
  if (sched.gtail)
    sched.gtail->next = gp;
  else
    sched.ghead = gp;
  sched.gtail = gp;
  sched.glen++;
}

void ready(G *gp) {
  for (; gp; gp = gp->next) {
    gp->state = G_RUNNABLE;
    gput(gp);
  }
}

void yield1(void) {
  ready(sched.g);
  yield0(G_RUNNABLE);
}

void yield(void) { yield0(G_WAITING); }

void yield0(int state) {
  G *gp;
  gp = sched.g;
  gp->state = state;
  if (gosave(&gp->ctx))
    return;
  schedule();
}

/* One round of scheduling */
void schedule(void) {
  G *gp;
  long ns;

retry:
  ns = timerscheck(&sched.t);
  gp = gget();
  if (!gp) {
    gp = netpoll(ns);
    if (gp) {
      ready(gp->next);
      gp->next = nil;
    }
  }
  if (!gp)
    goto retry;

  resume(gp);
}

static void resume(G *gp) {
  if ((char *)gp->ctx.sp - (char *)gp->stackguard < 256)
    throw("resume: stack overflow");
  sched.g = gp;
  gp->state = G_RUNNING;
  gogo(&gp->ctx);
}

static void await0(void *arg, long delay) {
  Event *ep;
  ep = (Event *)arg;
  /* Mark as expired for netpoll to ignore this event when it's ready */
  ep->fd = -1;
  ep->g->mp = ETIMEDOUT;
  gput(ep->g);
}

int await(int fd, int mode, long ns, long period) {
  G *gp;
  Event *ep;

  gp = sched.g;
  if (!gp)
    throw("await: sched.g is nil");

  ep = &gp->ev;
  if (ns > -1) {
    ep->period = period;
    ep->when = timens() + ns;
    ep->f = (void *)await0;
    ep->arg = (void *)ep;
    timersopen(&sched.t, ep);
  }
  if (fd > -1) {
    ep->fd = fd;
    ep->mode = mode;
    netpollopen(ep);
  }
  yield();
  return gp->mp;
}

void asleep(long ms) { await(-1, 0, ms * 1000000, 0); }