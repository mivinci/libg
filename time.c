#include <stdlib.h>

#include "runtime.h"

static int less(Event *, Event *);
static void up(Event **, int);
static int down(Event **, int, int);
static void push(Event **, Event *, int);
static Event *pop(Event **, int);


void timersinit(Timers *tp) {
  tp->heap = malloc(64);
  if (!tp->heap)
    throw("timersinit: out of memory");
  tp->len = 0;
  tp->cap = 64;
}

long timerscheck(Timers *tp) {
  Event *top;
  long now, delay, size;
  if (tp->len == 0)
    return -1;
  now = timens();
  if (tp->len > 0) {
    if (tp->cap - tp->len > 64) {
      size = tp->len;
      tp->heap = realloc(tp->heap, size);
      tp->cap = size;
    }
    for (; tp->len > 0;) {
      top = tp->heap[0];
      delay = now - top->when;
      if (delay < 0)
        return -delay;
      if (top->period > 0) {
        top->when += top->period + delay;
        down(tp->heap, 0, tp->len);
      } else {
        pop(tp->heap, tp->len);
        tp->len--;
      }
      top->f(top->arg, delay);
    }
  }
  return -1;
}

void timersopen(Timers *tp, Event *ep) {
  int size;
  if (tp->len >= tp->cap) {
    size = tp->cap * 3 / 2; /* 1.5x */
    tp->heap = realloc(tp->heap, size);
    tp->cap = size;
  }
  push(tp->heap, ep, tp->len);
  tp->len++;
}

static int less(Event *a, Event *b) {
  return a->when < b->when;
}

static void up(Event **h, int i) {
  Event *tmp;
  int j;
  for (;;) {
    j = (i - 1) / 2; /* parent */
    if (j == i || less(h[j], h[i]))
      break;
    tmp = h[i];
    h[i] = h[j];
    h[j] = tmp;
    i = j;
  }
}

static int down(Event **h, int i, int n) {
  Event *tmp;
  int t = i, j, j1, j2;
  for (;;) {
    j1 = 2 * i + 1; /* left child */
    if (j1 >= n)
      break;
    j = j1;

    j2 = j1 + 1; /* right child */
    if (j2 < n && less(h[j2], h[j1]))
      j = j2;

    if (less(h[i], h[j]))
      break;
    tmp = h[i];
    h[i] = h[j];
    h[j] = tmp;
    i = j;
  }
  return i > t;
}

static void push(Event **h, Event *ep, int n) {
  h[n] = ep;
  up(h, n);
}

static Event *pop(Event **h, int n) {
  Event *ep;
  if (n <= 0)
    return NULL;
  ep = h[0];
  h[0] = h[n - 1];
  down(h, 0, n);
  return ep;
}
