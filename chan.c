#include "runtime.h"

typedef struct Chan Chan;
typedef struct WaitQ WaitQ;
typedef struct Case Case;

struct Case {
  G *g;
  Chan *c;
  int op;
};

struct WaitQ {
  G *head;
  G *tail;
};

struct Chan {
  void *ptr;
  int len;
  int cap;
  int elsize;
  int closed;
  int sendx;
  int recvx;
  WaitQ sendq;
  WaitQ recvq;
};

Chan *makechan(int cap, int elsize) {}
int chansend(Chan *cp, void *p, bool block) {}
int chanrecv(Chan *cp, void *p, bool block) {}
void chanclose(Chan *cp) {}
int selectgo(Case *cas, int n, bool block) {}
