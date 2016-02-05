/*
 * ts7200.h - definitions describing the ts7200 peripheral registers
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */

#define TIMER_1         0x80810000
#define TIMER_2         0x80810020
#define TIMER_3         0x80810080
#define TIMER_4         0x8081005c
    #define LDR_OFFSET      0x00000000  // 16/32 bits, RW
    #define VAL_OFFSET      0x00000004  // 16/32 bits, RO
    #define CRTL_OFFSET     0x00000008  // 3 bits, RW
    #define ENABLE_MASK     0x00000080
    #define DEBUG_MASK      0x00000100
    #define MODE_MASK       0x00000040
    #define CLKSEL_MASK     0x00000008
    #define CLR_OFFSET      0x0000000c  // no data, WO

#define INT_1           0x800B0000
#define INT_2           0x800C0000
    #define STATUS_OFFSET   0x00
    #define ENABLE_OFFSET   0x10
    #define EN_CLEAR_OFFSET 0x14  

#define UART_1          0x808c0000
#define UART_2          0x808d0000

#define UART_SPEED_LO 0xBF
#define UART_SPEED_HI 0x3

#define UART_DATA_OFFSET    0x0 // low 8 bits
    #define DATA_MASK   0xff
#define UART_RSR_OFFSET     0x4 // low 4 bits
    #define FE_MASK     0x1
    #define PE_MASK     0x2
    #define BE_MASK     0x4
    #define OE_MASK     0x8
#define UART_LCRH_OFFSET    0x8 // low 7 bits
    #define BRK_MASK    0x1
    #define PEN_MASK    0x2 // parity enable
    #define EPS_MASK    0x4 // even parity
    #define STP2_MASK   0x8 // 2 stop bits
    #define FEN_MASK    0x10    // fifo
    #define WLEN_MASK   0x60    // word length
#define UART_LCRM_OFFSET    0xc // low 8 bits
    #define BRDH_MASK   0xff    // MSB of baud rate divisor
#define UART_LCRL_OFFSET    0x10    // low 8 bits
    #define BRDL_MASK   0xff    // LSB of baud rate divisor
#define UART_CTLR_OFFSET    0x14    // low 8 bits
    #define UARTEN_MASK 0x1
    #define MSIEN_MASK  0x8 // modem status int
    #define RIEN_MASK   0x10    // receive int
    #define TIEN_MASK   0x20    // transmit int
    #define RTIEN_MASK  0x40    // receive timeout int
    #define LBEN_MASK   0x80    // loopback 
#define UART_FLAG_OFFSET    0x18    // low 8 bits
    #define CTS_MASK    0x1
    #define DCD_MASK    0x2
    #define DSR_MASK    0x4
    #define TXBUSY_MASK 0x8
    #define RXFE_MASK   0x10    // Receive buffer empty
    #define TXFF_MASK   0x20    // Transmit buffer full
    #define RXFF_MASK   0x40    // Receive buffer full
    #define TXFE_MASK   0x80    // Transmit buffer empty
#define UART_INTR_OFFSET    0x1c
#define UART_DMAR_OFFSET    0x28

// Specific to UART1

#define UART_MDMCTL_OFFSET  0x100
    #define RTS_MASK    0x1
#define UART_MDMSTS_OFFSET  0x104
#define UART_HDLCCTL_OFFSET 0x20c
#define UART_HDLCAMV_OFFSET 0x210
#define UART_HDLCAM_OFFSET  0x214
#define UART_HDLCRIB_OFFSET 0x218
#define UART_HDLCSTS_OFFSET 0x21c
