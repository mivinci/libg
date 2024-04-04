#include <errno.h>
#include <fcntl.h>

#include "runtime.h"

void setcloexec(int fd) {
}

void setnonblock(int fd) {
  int n;
  n = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
  if (n < 0)
    throw("setnonblock: fcntl failed with errno %d", errno);
}


