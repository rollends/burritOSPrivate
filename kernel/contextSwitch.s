.section .text
.global kernelStart
.global systemCall

kernelStart:                        @ Called after boostrapping
    stmfd   sp!, {r4-r12, lr}       @ Stack the kernel registers

enterTask:                          @ Enters a user task with an sp in r0
    msr     cpsr_c, #0xDF           @ Switch to system mode
    ldmfd   r0!, {r1, lr}           @ Load task PSR and LR from the task sp
    add     sp, r0, #44             @ Set the task sp

    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    msr     spsr, r1                @ Update the SPSR with the task CPSR
    ldmfd   r0, {r0, r4-r12, pc}^   @ Load task registers

systemCall:                         @ Called when an SWI occurs
    msr     cpsr_c, #0xDF           @ Switch to system mode

    stmfd   sp!, {lr}               @ Stack the LR
    bl kernelSystemCall             @ Branch to the system call
    ldmfd   sp, {lr}                @ Restore the LR

    @ The sp above does not use writeback because we're making space to store
    @ the pc. We store the PC later, but need to leave a spot on the stack for
    @ it, and thus simply don't use writeback when popping the lr.

    stmfd   sp!, {r0, r4-r12}       @ Store task registers to task stack
    mov     r0, sp                  @ Store task sp
    mov     r2, lr                  @ Store task lr
    
    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    mrs     r1, spsr                @ Retrieve the user process PSR
    stmfd   r0!, {r1, r2}           @ Save the task PSR and LR to the task stack
    str     lr, [r0, #48]           @ Store the pc to the task stack

    bl kernelSchedule               @ Schedule next process and update the sp

    cmp r0, #0                      @ Check if the kernel should exit
    bne enterTask                   @ Enter next task

kernelEnd:                          @ Returns to kernel main and exits
    ldmfd   sp!, {r4-r12, pc}       @ Restore the kernel registers
