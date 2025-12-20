#ifndef ARCH_TYPES_H
#define ARCH_TYPES_H

/*
 * architecture-independent type definitions.
 * 
 * common types are in lib/types.h (exact width, limits, bool, NULL)
 * each architecture adds its specific types in arch/<arch>/types.h:
 *
 * ARCH_BITS - 32 or 64
 * ARCH_PTR_SIZE - sizeof(void*) in bytes
 * intptr, uintptr - pointer-sized integers
 * size, ssize - size types
 * word, uword - native register width
 */

#if defined(ARCH_AMD64)
    #include <arch/amd64/types.h>
#elif defined(ARCH_X86)
    #error "x86 not implemented"
#elif defined(ARCH_ARM64)
    #error "ARM64 not implemented"
#else
    #error "Unsupported architecture"
#endif

#endif
