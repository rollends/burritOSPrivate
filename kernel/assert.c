#include "kernel/assert.h"
#include "kernel/print.h"

void assertHandler(const U32 line, const char* file)
{
    printTaco();
    printBlocking("Assertion in `%s` on line %d\r\n\r\n", file, line);
    while (1)
    {
        
    }
}
