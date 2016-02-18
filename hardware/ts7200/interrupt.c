#include "hardware/interrupt.h"
#include "hardware/memory.h"
#include "hardware/ts7200/ts7200.h"

RETURN interruptEnable(const U32 interrupt, const U32 bits)
{
    RWRegister enable = (RWRegister)(interrupt + ENABLE_OFFSET);
    U32 values = __ldr(enable);
    values |= bits;
    __str(enable, values);

    IS_OK();
}

RETURN interruptClear(const U32 interrupt, const U32 bits)
{
    RWRegister clear = (RWRegister)(interrupt + EN_CLEAR_OFFSET);
    __str(clear, bits);

    IS_OK();
}

RETURN interruptStatus(const U32 interrupt, U32* status)
{
    RORegister stat = (RORegister)(interrupt + STATUS_OFFSET);
    *status = __ldr(stat);

    IS_OK();
}
