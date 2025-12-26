bits 64
global enable_sse

enable_sse:
    ; CR0: clear EM (bit 2), set MP (bit 1)
    mov     rax, cr0
    and     ax, 0xFFFB        ; clear bit 2 (EM)
    or      ax, 0x0002        ; set bit 1 (MP)
    mov     cr0, rax

    ; CR4: set OSFXSR (bit 9) and OSXMMEXCPT (bit 10)
    mov     rax, cr4
    or      ax, (1 << 9) | (1 << 10)
    mov     cr4, rax

    ret
