#include "gg.h"

namespace gg {
void init(void) { gg_init(); }
void wait(void) { gg_wait(); }
void spawn(void (*f)(void *), void *arg, int size) { gg_spawn(f, arg, size); }
void yield(void) { gg_yield(); }
void yield1(void) { gg_yield1(); }
void sleep(long ms) { gg_sleep(ms); }
} // namespace gg
