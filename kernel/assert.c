#include "kernel/assert.h"
#include "kernel/print.h"
#include "kernel/kernelData.h"

void assertHandler(const U32 line, const char* file)
{
    printTaco();
    printBlocking("Assertion in `%s` on line %d\r\n\r\n", file, line);
    printBlocking("Current Active Task is %x\r\n", kernel.activeTask->tid.value);
    while (1)
    {
        
    }
}
