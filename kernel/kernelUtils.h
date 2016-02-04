#ifndef KERNEL_UTIL_H
#define KERNEL_UTIL_H

/**
 * Exits a task via a system call
 */
extern U32 __taskExit();

/**
 * Invokes a system call with a syscall id and 3 extra parameters
 */
extern U32 __sysCall3(U32, U32, U32, U32);

/**
 * Invokes a system call with a syscall id and 2 extra parameters
 */
extern U32 __sysCall2(U32, U32, U32);

/**
 * Invokes a system call with a syscall id and 1 extra parameter
 */
extern U32 __sysCall1(U32, U32);

/**
 * Invokes a system call with a syscall id and no extra parameters
 */
extern U32 __sysCall0(U32);

#endif
