.section .vector
.global _vectorLoad

vectorTable:
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
    swi_handler_ref:        .word _systemCall
    prefetch_handler_ref:   .word hang
    data_handler_ref:       .word hang
    unused_handler_ref:     .word hang
    irq_handler_ref:        .word _interruptCall
    fiq_handler_ref:        .word hang

_vectorLoad:
    ldr r1, =vectorTable
    mov r2, #0

    ldmia r1!,{r3,r4,r5,r6,r7,r8,r9,r10}
    stmia r2!,{r3,r4,r5,r6,r7,r8,r9,r10}
    ldmia r1,{r3,r4,r5,r6,r7,r8,r9,r10}
    stmia r2,{r3,r4,r5,r6,r7,r8,r9,r10}
    bx lr

hang:
    b hang
