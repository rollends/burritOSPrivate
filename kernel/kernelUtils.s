.section .text
.global __taskExit
.global __sysCall0
.global __sysCall1
.global __sysCall2
.global __sysCall3

__taskExit:
    mov r0, #0
    swi 0

__sysCall0:
    swi 0
    bx lr

__sysCall1:
    swi 0
    bx lr

__sysCall2:
    swi 0
    bx lr

__sysCall3:
    swi 0
    bx lr
