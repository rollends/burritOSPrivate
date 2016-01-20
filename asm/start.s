.section .init
.global _start
_start:
    sub r0, pc, #8
    stmfd sp!, {lr}

    msr cpsr_c, #0xD3
    @mov sp,#0x8000000

    bl vector_load
    bl main

    ldmfd sp!, {pc}

_vector_table:
    ldr pc,reset_handler_ref
    ldr pc,undefined_handler_ref
    ldr pc,swi_handler_ref
    ldr pc,prefetch_handler_ref
    ldr pc,data_handler_ref
    ldr pc,unused_handler_ref
    ldr pc,irq_handler_ref
    ldr pc,fiq_handler_ref

    reset_handler_ref:      .word _start
    undefined_handler_ref:  .word hang
    swi_handler_ref:        .word swi_handler
    prefetch_handler_ref:   .word hang
    data_handler_ref:       .word hang
    unused_handler_ref:     .word hang
    irq_handler_ref:        .word hang
    fiq_handler_ref:        .word hang

vector_load:
    ldr r1, = _vector_table
    add r1, r1, r0
    mov r2, #0

    ldmia r1!,{r3,r4,r5,r6,r7,r8,r9,r10}
    stmia r2!,{r3,r4,r5,r6,r7,r8,r9,r10}
    ldmia r1,{r3,r4,r5,r6,r7,r8,r9,r10}

    add r3, r3, r0
    add r4, r4, r0
    add r5, r5, r0
    add r6, r6, r0
    add r7, r7, r0
    add r8, r8, r0
    add r9, r9, r0
    add r10, r10, r0

    stmia r2,{r3,r4,r5,r6,r7,r8,r9,r10}
    bx lr

hang:
    b hang

swi_handler:
    b enterKernelR


.global sys_call
sys_call:
    swi 0
    bx lr
    
