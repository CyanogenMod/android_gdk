#include <unistd.h>
#include <sys/socket.h>
#include <sys/linux-syscalls.h>

#define SOCK_STREAM_PORTABLE 1
#define SOCK_DGRAM_PORTABLE 2

#if SOCK_STREAM==SOCK_STREAM_PORTABLE
#error Bad build environment
#endif

static inline int mips_change_type(int type)
{
    switch (type) {
      case SOCK_DGRAM_PORTABLE: return SOCK_DGRAM;
      case SOCK_STREAM_PORTABLE: return SOCK_STREAM;
    }
    return type;
}

extern int socket(int, int, int);

int socket_portable(int domain, int type, int protocol) {
    return socket(domain,mips_change_type(type),protocol);
}
