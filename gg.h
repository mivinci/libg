#ifndef GG_H
#define GG_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__)
#include <stdbool.h>
#else
#define bool int
#define true 1
#define false 0
#endif

#define gg_panic(...) gg_throw(__FILE__, __LINE__, __VA_ARGS__)
void gg_throw(const char *, int, const char *, ...);

#define gg(f, a)                                                               \
  gg_spawn((void (*)(void *))(f), (void *)(unsigned long)(a), 4096)

void gg_init(void);
void gg_wait(void);
void gg_spawn(void (*)(void *), void *, int);
void gg_yield(void);
void gg_yield1(void);
void gg_sleep(long);

#define gg_makechan(t, n) gg_makechansize(n, sizeof(t))

struct gg_chan;
struct gg_chan *gg_makechansize(int, int);
bool gg_send(struct gg_chan *, void *);
bool gg_recv(struct gg_chan *, void *);
void gg_close(struct gg_chan *);

#define GG_UDP 1
#define GG_TCP 2

int gg_dial(int, const char *, unsigned short);
int gg_listen(int, const char *, unsigned short);
int gg_dialunix(int, const char *);
int gg_listenunix(int, const char *);


#ifdef __cplusplus
}
#endif

#endif
