.section .init
.global _interruptCall
.global _start
.global _systemCall

@
@ Entry point to the kernel, called by Redboot when the kernel starts. This
@ turn on caches, initializes the vector table and bootstraps the kernel. The
@ kernelBoostrap function returns the stack pointer of the first user task to
@ run, so control falls through to enterTask below.
@
_start:
    sub     r0, pc, #8              @ Store off the PC
    msr     cpsr_c, #0xD3           @ Disable interupts
    stmfd   sp!, {lr}               @ Store the LR for when we exit the kernel 

    bl      enableICache            @ Enable the instruction cache
    bl      enableDCache            @ Enable the data cache

    bl      _vectorLoad             @ Load the vector table
    bl      kernelBoostrap          @ Boostrap the kernel

@
@ Entry point for a user task. R0 should contain the stack pointer for the task
@ [r0 0] is the PSR, [r0 1] is the task LR and [r0 2] - [r0 15] are r0-r12, pc.
@ This code block restores the PSR and jumps to the task PC.
@
enterTask:                          @ Enter a user task with an sp in r0
    msr     cpsr_c, #0xDF           @ Switch to system mode
    ldmfd   r0!, {r1, lr}           @ Load task PSR and LR from the task sp
    add     sp, r0, #56             @ Set the task sp

    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    msr     spsr, r1                @ Update the SPSR with the task CPSR
    ldmfd   r0, {r0-r12, pc}^       @ Load task registers & enter the task

@
@ Called when a hardware interrupt occurs. The task state is stored immediately
@ to the task stack and then the kernelInterrupt function is called to handle 
@ any processing needed for the interrupt. The task LR and SPSR are fetched and
@ stored in IRQ mode, then the kernelSchedule is called in SVC mode and a new 
@ task is entered
@
_interruptCall:                     @ Called when an hardware interrupt occurs
    msr     cpsr_c, #0xDF           @ Switch to system mode

    sub     sp, sp, #4              @ Make space for the PC
    stmfd   sp!, {r0-r12}           @ Stack most user registers
    mov     r4, lr                  @ Save off the lr
    bl      kernelInterrupt         @ Branch to the system call
    mov     r0, sp                  @ Store task sp
    
    msr     cpsr_c, #0xD2           @ Switch to IRQ mode
    mrs     r1, spsr                @ Retrieve the user process PSR
    stmfd   r0!, {r1, r4}           @ Save the task PSR and LR to the task stack
    sub     lr, lr, #4              @ Offset the task by pc
    str     lr, [r0, #60]           @ Store the pc to the task stack

    msr     cpsr_c, #0xD3           @ Switch to supervisor mode
    bl      kernelSchedule          @ Schedule next task and update old task sp

    cmp     r0, #0                  @ Check if the kernel should exit
    bne     enterTask               @ Enter next task
    b       kernelEnd               @ Exit - should not ever occur in practice

@
@ Called when a sofware interrupt (system call) occurs. Task state is stored 
@ immediately to the task stack and then the kernelSystemCall function is called
@ to handle the system call. R0 is the system call id, r1-r3 are parameters and
@ additional parameters are found on the user stack. The task LR and PSR are
@ fetched and stored in SVC mode, then a new task is scheduled and entered.
@
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

@
@ Exits the kernel by restoring the saved LR into the PC, thus returning to the 
@ Redboot code
@
kernelEnd:                          @ Exists the kernel
    bl      kernelCleanup           @ Cleanup the kernel
    ldmfd   sp!, {pc}               @ Load the LR into PC to return to redboot

@
@ Enables the instruction cache
@
enableICache:
    mrc     p15, 0, r1, c1, c1, 0   @ Read control config 
    orr     r1, r1, #0x1000         @ Toggle i-cache bit
    mcr     p15, 0, r1, c1, c1, 0   @ Save control config
    bx lr

@
@ Enables the data cache
@
enableDCache:
    mrc     p15, 0, r1, c1, c1, 0   @ Read control config           
    orr     r1, r1, #0x5            @ Toggle d-cache and mmu bits
    mcr     p15, 0, r1, c1, c1, 0   @ Save control config
    bx lr
