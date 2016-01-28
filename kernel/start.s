.section .init
.global _start
.global _systemCall

_start:
    sub     r0, pc, #8              @ Store off the PC
    msr     cpsr_c, #0xD3           @ Disable interupts

    stmfd   sp!, {lr}               @ Store the LR for when we exit the kernel 

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
    mov     r10, lr                 @ Save off the lr
    bl      kernelSystemCall        @ Branch to the system call
    mov     lr, r10                 @ Restore the lr
    stmfd   sp!, {r0}               @ Store R0 so the return value is recorded

    mov     r0, sp                  @ Store task sp
    mov     r2, lr                  @ Store task lr
    
    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    mrs     r1, spsr                @ Retrieve the user process PSR
    stmfd   r0!, {r1, r2}           @ Save the task PSR and LR to the task stack
    str     lr, [r0, #60]           @ Store the pc to the task stack

    bl      kernelSchedule          @ Schedule next task and update old task sp

    cmp     r0, #0                  @ Check if the kernel should exit
    bne     enterTask               @ Enter next task

kernelEnd:                          @ Exists the kernel
    ldmfd   sp!, {pc}               @ Load the LR into PC to return to redboot
