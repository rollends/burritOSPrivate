.section .init
.global _start
.global _systemCall

_start:
    sub     r0, pc, #8              @ Store off the PC
    msr     cpsr_c, #0xD3           @ Disable interupts
    stmfd   sp!, {lr}               @ Store the LR for when we exit the kernel 

    bl      enableICache            @ Enable the instruction cache
    bl      enableDCache            @ Enable the data cache

    bl      _vectorLoad             @ Load the vector table
    bl      kernelBoostrap          @ Boostrap the kernel

enterTask:                          @ Enter a user task with an sp in r0
    msr     cpsr_c, #0xDF           @ Switch to system mode
    ldmfd   r0!, {r1, lr}           @ Load task PSR and LR from the task sp
    add     sp, r0, #56             @ Set the task sp

    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    msr     spsr, r1                @ Update the SPSR with the task CPSR
    ldmfd   r0, {r0-r12, pc}^       @ Load task registers & enter the task

_systemCall:                        @ Called when an SWI occurs
    msr     cpsr_c, #0xDF           @ Switch to system mode

    sub     sp, sp, #4              @ Make space for the PC
    stmfd   sp!, {r1-r12}           @ Stack most user registers
    mov     r4, lr                  @ Save off the lr
    bl      kernelSystemCall        @ Branch to the system call
    mov     r5, r0                  @ Save the return value
    mov     r0, sp                  @ Store task sp
    
    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    mrs     r1, spsr                @ Retrieve the user process PSR
    stmfd   r0!, {r1, r4, r5}       @ Save the task PSR and LR to the task stack
    str     lr, [r0, #60]           @ Store the pc to the task stack

    bl      kernelSchedule          @ Schedule next task and update old task sp

    cmp     r0, #0                  @ Check if the kernel should exit
    bne     enterTask               @ Enter next task

kernelEnd:                          @ Exists the kernel
    ldmfd   sp!, {pc}               @ Load the LR into PC to return to redboot

enableICache:
    mrc     p15, 0, r1, c1, c1, 0   @ Read control config 
    orr     r1, r1, #0x1000         @ Toggle i-cache bit
    mcr     p15, 0, r1, c1, c1, 0   @ Save control config
    bx lr

enableDCache:
    mrc     p15, 0, r1, c1, c1, 0   @ Read control config           
    orr     r1, r1, #0x5            @ Toggle d-cache and mmu bits
    mcr     p15, 0, r1, c1, c1, 0   @ Save control config
    bx lr
