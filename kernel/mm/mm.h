#ifndef MM_MM_H
#define MM_MM_H

#include <arch/types.h>

//higher half firect map (HHDM) offset
//all physical memory is mapped starting at this virtual address
#define HHDM_OFFSET 0xFFFF800000000000ULL

//physical to virtual conversion using HHDM
#define P2V(phys) ((void *)((uintptr)(phys) + HHDM_OFFSET))

//virtual to physical conversion using HHDM
#define V2P(virt) ((uintptr)(virt) - HHDM_OFFSET)

#endif
