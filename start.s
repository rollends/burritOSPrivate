.section .init
.global _start
.global _
_start:
    sub r0, pc, #8
    msr cpsr_c, #0xD3
    bl main
