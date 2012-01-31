#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

extern int  __open(const char*, int, int);
#define O_CREAT_PORTABLE 00000100

static inline int mips_change_flags(int flags) {
    if (flags & O_CREAT_PORTABLE) {
        flags &= ~O_CREAT_PORTABLE;
        flags |= O_CREAT;
    }
    return flags;
}

int open(const char *pathname, int flags, ...) {
    mode_t  mode = 0;
    flags = mips_change_flags(flags);
    flags |= O_LARGEFILE;

    if (flags & O_CREAT) {
        va_list  args;

        va_start(args, flags);
        mode = (mode_t) va_arg(args, int);
        va_end(args);
    }

    return __open(pathname, flags, mode);
}
