#include "common/types.h"
#include "hardware/hardware.h"
#include "kernel/kernelData.h"
#include "kernel/print.h"

#include "user/IdleTask.h"

U32* bootstrap(U32 pc)
{
    uartSpeed(UART_2, UART_SPEED_HI);
    uartConfig(UART_2, 0, 0, 0);
    
    timerInit(TIMER_3); 
    timerSetValue(TIMER_3, 508000);
    timerClear(TIMER_3);

    interruptEnable(INT_2, 0x80000);

    printString("%c[2J\r", 27);

    kernelDataInit(pc);

    U16 taskID = taskTableAlloc(&kernel.tasks,
                                31,
                                (U32)(&IdleTask) + pc,
                                VAL_TO_ID(0));

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    return kernel.activeTask->stack;
}

void cleanup()
{
    interruptClear(INT_1, 0xFFFFFFFF);
    interruptClear(INT_2, 0xFFFFFFFF);
}
