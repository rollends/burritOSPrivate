#ifndef UART_H
#define UART_H

#include "ts7200.h"

#define UART_PORT_1 UART1_BASE
#define UART_PORT_2 UART2_BASE

#define UART_SPEED_LO 0xBF
#define UART_SPEED_HI 0x3

/// Sets the transfer speed for a given UART port
int uartSpeed(const int uart, const int speed);

/// Sets uart frame configuration
int uartConfig(const int uart, const int fifo, const int stp, const int pen);

/// Blocking write to the UART
int uartWriteChar(const int uart, const char c);

/// Blocking ready from the UART
int uartReadChar(const int uart, char* c);

#endif

