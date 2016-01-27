.section .text
.global enterKernelR
.global enterKernelT
.global enterTask

@ 
@ Returns control from a task to the kernel via a system call. The system call
@ ID is stored in R0. Registers R1 - R3 contain system call parameters. System
@ calls that require more than three parameters should use the `enterKernelS`
@ switch that loads additional parameters from the user stack
@
@ The new stack pointer is returned to the kernel by this method
@
enterKernelR:
    msr     cpsr_c, #0xDF           @ Switch to system mode

    stmfd   sp!, {lr}               @ Stack the LR
    bl systemCall                   @ Branch to the system call
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

    bl systemSchedule               @ Schedule next process. SP is updated
    cmp r0, #0                      @ Check if the kernel should exit
    beq exitKernel                  @ Branch out
    b enterTaskInner                @ Enter next task

exitKernel:
    ldmfd   sp!, {r4-r12, pc}       @ Restore the kernel registers


@
@ C function:   U32* enterTask(U32* sp)
@ 
@ Enters the task with the stack pointer `sp`.
@
enterTask:
    stmfd   sp!, {r4-r12, lr}           @ Store the kernel registers

enterTaskInner:
    msr     cpsr_c, #0xDF               @ Switch to system mode
    ldmfd   r0!, {r1, lr}               @ Load task PSR and LR from the task sp
    add     sp, r0, #44                 @ Set the task sp

    msr     cpsr_c, #0xD3               @ Switch to supervisor mode
    msr     spsr, r1                    @ Update the SPSR with the task CPSR
    ldmfd   r0, {r0, r4-r12, pc}^       @ Load task registers
