#ifndef G_H
#define G_H

#define go(f, a) gospawn((void (*)(void *))f, a)

void goinit(void);
void gowait(void);
void gospawn(void (*)(void *), void *);
void goyield(void);
void goyield1(void);
void gosleep(long);

#endif
