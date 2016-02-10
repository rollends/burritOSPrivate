#include "hardware/interrupt.h"
#include "hardware/memory.h"
#include "hardware/ts7200/ts7200.h"

S32 interruptEnable(const U32 interrupt, const U32 bits)
{
    RWRegister enable = (RWRegister)(interrupt + ENABLE_OFFSET);
    U32 values = __ldr(enable);
    values |= bits;
    __str(enable, values);

    return 0;
}

S32 interruptClear(const U32 interrupt, const U32 bits)
{
    RWRegister clear = (RWRegister)(interrupt + EN_CLEAR_OFFSET);

    __str(clear, bits);

    return 0;
}

U32 interruptStatus(const U32 interrupt)
{
    RORegister status = (RORegister)(interrupt + STATUS_OFFSET);
    return __ldr(status);
}
