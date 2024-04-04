#ifndef RUNTIME_H
#define RUNTIME_H

#define nil ((void *)0)

#define G_IDLE 0
#define G_RUNNABLE 1
#define G_RUNNING 2
#define G_WAITING 3
#define G_DEAD 4

#define EV_READ 1
#define EV_WRITE 2

typedef struct G G;
typedef struct Event Event;
typedef struct Gobuf Gobuf;
typedef struct Timers Timers;
typedef struct Sched Sched;

struct Gobuf {
  void *sp;
  void *pc;
  void *gr[8];
};

struct Event {
  int fd;
  int mode;
  long when;
  long period;
  void (*f)(void *, long);
  void *arg;
  G *g;
};

struct Timers {
  Event **heap;
  int len;
  int cap;
};

struct G {
  void *stackguard;
  G *next;
  Gobuf ctx;
  Event ev;
  int state;
  int mp;
};

struct Sched {
  G *gx; /* current g running on this scheduler */
  G *gfree;
  G *ghead;
  G *gtail;
  Timers t;
};

void throw(const char *, ...);
void setcloexec(int);
void setnonblock(int);
void yield(void);
void yield1(void);
void schedule(void);
void spawn(void (*)(void *), void *, int);
int await(int, int, long, long);
void asleep(long);
void schedinit(void);
void netpollinit(void);
int netpollopen(Event *);
G *netpoll(long);
long timens(void);
void timersinit(Timers *);
void timersopen(Timers *, Event *);
long timerscheck(Timers *);
void hookinit(void);

#endif
