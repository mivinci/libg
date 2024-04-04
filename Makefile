AR     := ar
CC     := gcc
CFLAGS := -Wall
OS     := $(shell uname -s)
ARCH   := $(shell uname -m)

OBJS := sched.o netpoll.o time.o net.o hook.o g.o

ifeq ($(OS), Linux)
	OBJS += netpoll_linux.o time_linux.o
endif

ifeq ($(OS), Darwin)
	OBJS += netpoll_freebsd.o time_freebsd.o
endif

ifeq ($(ARCH), x86_64)
	OBJS += sched_amd64.o
endif

%.o: %.S
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

libg.a: $(OBJS)
	$(AR) rcs $@ $^

all: libg.a

clean:
	rm *.o
	rm *.a
