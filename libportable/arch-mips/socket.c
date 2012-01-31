#include <unistd.h>
#include <sys/socket.h>
#include <sys/linux-syscalls.h>

static inline int mips_change_type(int type) {
    switch (type) {
      case SOCK_DGRAM: return 2;
      case SOCK_STREAM: return 1;
    }
    return type;
}

extern int socket(int, int, int);

int socket_portable(int domain, int type, int protocol) {
    return socket(domain,mips_change_type(type),protocol);
}
