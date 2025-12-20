#ifndef ARCH_AMD64_TYPES_H
#define ARCH_AMD64_TYPES_H

//common types first
#include <lib/types.h>

//architecture info
#define ARCH_BITS     64
#define ARCH_PTR_SIZE 8

//pointer-sized integers (64-bit on amd64)
typedef int64  intptr;
typedef uint64 uintptr;

//size types (64-bit on amd64)
typedef uint64 size;
typedef int64  ssize;

//native word types (64-bit on amd64)
typedef int64  word;
typedef uint64 uword;

//maximum-width integer types
typedef int64  intmax;
typedef uint64 uintmax;

//pointer-sized limits
#define INTPTR_MIN  INT64_MIN
#define INTPTR_MAX  INT64_MAX
#define UINTPTR_MAX UINT64_MAX
#define SIZE_MAX    UINT64_MAX
#define SSIZE_MAX   INT64_MAX

#endif
