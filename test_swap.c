#include <stdio.h>
#include <stdlib.h>
#include "runtime.h"

extern int gosave(Gobuf *) asm("gosave");
extern int gogo(Gobuf *) asm("gogo");

Gobuf b;

int main(void) {
  if (gosave(&b)) {
    printf("gosave 1");
    exit(0);
  }

  printf("gosave 0");
  gogo(&b);
}
