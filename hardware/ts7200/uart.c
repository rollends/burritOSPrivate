#include "hardware/memory.h"
#include "hardware/uart.h"
#include "hardware/ts7200/ts7200.h"

S32 uartEnable(const U32 uart,
               const U32 enable,
               const U32 timeout,
               const U32 trans,
               const U32 rcv,
               const U32 modem)
{
    RWRegister control = (RWRegister)(uart + UART_CTLR_OFFSET);
    
    U32 value = __ldr(control);
    value = (enable ? value | UARTEN_MASK : value & ~UARTEN_MASK);
    value = (modem ? value | MSIEN_MASK : value & ~MSIEN_MASK);
    value = (rcv ? value | RIEN_MASK : value & ~RIEN_MASK);
    value = (trans ? value | TIEN_MASK : value & ~TIEN_MASK);
    value = (timeout ? value | RTIEN_MASK : value & ~RTIEN_MASK);
    __str(control, value);

    return 0;
}

S32 uartSpeed(const U32 uart, const U32 speed)
{
    RWRegister mid = (RWRegister)(uart + UART_LCRM_OFFSET);
    RWRegister low = (RWRegister)(uart + UART_LCRL_OFFSET);

    __str(mid, 0x0);
    __str(low, speed);

    return 0;
}

S32 uartConfig(const U32 uart,
               const U32 fifo,
               const U32 stp,
               const U32 pen)
{
    RWRegister high = (RWRegister)(uart + UART_LCRH_OFFSET);

    U32 value = __ldr(high);
    value = (fifo ? value | FEN_MASK : value & ~FEN_MASK);
    value = (stp ? value | STP2_MASK : value & ~STP2_MASK);
    value = (pen ? value | PEN_MASK : value & ~PEN_MASK);
    value = value | WLEN_MASK;
    __str(high, value);
    
    return 0;
}

S32 uartWriteByteBlock(const U32 uart, const U8 byte)
{
    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (*flags & TXFF_MASK)
    {
    }

    __strb(data, byte);
    return 0;
}

S32 uartWriteByte(const U32 uart, const U8 byte)
{
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);
    __strb(data, byte);
    return 0;
}

S32 uartReadByteBlock(const U32 uart, U8* byte)
{
    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (!(*flags & RXFF_MASK))
    {
    }

    *byte = __ldr(data);
    return 0;
}

S32 uartReadByte(const U32 uart, U8* byte)
{
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);
    *byte = __ldr(data);
    return 0;
}

U32 uartCTS(const U32 uart)
{
    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);

    return __ldr(flags) & CTS_MASK;
}

S32 uartInterruptTX(const U32 uart, const U8 enable)
{
    RWRegister control = (RWRegister)(uart + UART_CTLR_OFFSET);
    
    U32 value = __ldr(control);
    value = (enable ? value | TIEN_MASK : value & ~TIEN_MASK);
    __str(control, value);

    return 0;
}

U8 uartInterruptStatus(const U32 uart)
{
    RWRegister status = (RWRegister)(uart + UART_INTR_OFFSET);
    U8 result = __ldr(status) & 0xF;
    __str(status, 0);

    return result;
}
