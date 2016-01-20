#ifndef UART_H
#define UART_H

#include "ts7200.h"
#include "types.h"

#define UART_PORT_1 UART1_BASE
#define UART_PORT_2 UART2_BASE

#define UART_SPEED_LO 0xBF
#define UART_SPEED_HI 0x3

/// Sets the transfer speed for a given UART port
S32 uartSpeed(const U32 uart, const U32 speed);

/// Sets uart frame configuration
S32 uartConfig(const U32 uart, const U32 fifo, const U32 stp, const U32 pen);

/// Blocking write to the UART
S32 uartWriteByte(const U32 uart, const U8 c);

/// Blocking read from the UART
S32 uartReadByte(const U32 uart, U8* c);

#endif
