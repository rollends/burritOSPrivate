#include "common/common.h"
#include "hardware/hardware.h"

#include "kernel/assert.h"
#include "kernel/kernelData.h"
#include "kernel/print.h"
#include "kernel/kernelUtils.h"

#include "user/IdleTask.h"

U32* bootstrap()
{
    // Initialize UART 2 (for terminal io) and clear the screen
    assertOk(uartEnable(UART_2, 1, 0, 0, 1, 0));
    assertOk(uartSpeed(UART_2, UART_SPEED_HI));
    assertOk(uartConfig(UART_2, 0, 0, 0));
    printBlocking("%c[2J", 27);
        
    // Initialize UART 1 (for train system io)
    assertOk(uartEnable(UART_1, 1, 0, 0, 1, 1));
    assertOk(uartSpeed(UART_1, UART_SPEED_LO));
    assertOk(uartConfig(UART_1, 0, 1, 0));
    
    // Start the 10ms timer and clear the IRQ
    assertOk(timerEnable(TIMER_1, 1, 1, 1));
    assertOk(timerSetValue(TIMER_1, 5080));
    assertOk(timerClear(TIMER_1));

    // Start the 150 ms timer and clear the IRQ
    assertOk(timerEnable(TIMER_2, 1, 1, 0));
    assertOk(timerSetValue(TIMER_2, 300));
    assertOk(timerClear(TIMER_2));

#ifdef KERNEL_PERF
    // Start the 983 kHz debug timer
    assertOk(timerEnable(TIMER_4, 1, 0, 0));
#endif
    
    // Enable timer, UART2_TX and UART2_RX interrupts
    assertOk(interruptEnable(INT_1, 0x06000030));

    // Enable the UART1_OR interrupt and clear the IRQ
    assertOk(interruptEnable(INT_2, 0x00100000));
    uartInterruptStatus(UART_1);

    // Initialize the kernel data
    assertOk(kernelDataInit());

    // Create the idle task and return the stack pointer
    U16 taskID = taskTableAlloc(&kernel.tasks,
                                PRIORITY_COUNT - 1,
                                (U32)(&IdleTask),
                                (U32)(&__taskExit),
                                VAL_TO_ID(0));

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    return kernel.activeTask->stack;
}

void cleanup()
{
    assertOk(interruptClear(INT_1, 0xFFFFFFFF));
    assertOk(interruptClear(INT_2, 0xFFFFFFFF));
}
