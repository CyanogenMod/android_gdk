#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>

/* May integate into one header file */
#define MAP_ANONYMOUS_PORTABLE 0x20
#define PROT_SEM_PORTABLE 0x8

extern void* __mmap2(void*, size_t, int, int, int, size_t);

#if MAP_ANONYMOUS_PORTABLE==MAP_ANONYMOUS
#error Bad build environment
#endif

static inline int mips_change_prot(int prot) {
    if (prot & PROT_SEM_PORTABLE) {
        prot &= ~PROT_SEM_PORTABLE;
        prot |= PROT_SEM;
    }

    return prot;
}

static inline int mips_change_flags(int flags) {
    if (flags & MAP_ANONYMOUS_PORTABLE) {
        flags &= ~MAP_ANONYMOUS_PORTABLE;
        flags |= MAP_ANONYMOUS;
    }

    return flags;
}

#define  MMAP2_SHIFT  12
void* mmap(void* addr, size_t size, int prot, int flags, int fd, long offset) {
    if ( offset & ((1UL << MMAP2_SHIFT)-1) ) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    prot = mips_change_prot(prot);
    flags = mips_change_flags(flags);
    return __mmap2(addr, size, prot, flags, fd, (size_t)offset >> MMAP2_SHIFT);
}
