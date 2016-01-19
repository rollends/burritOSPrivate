#include "uart.h"

#define UART0_BASE   0x20201000
#define UART0_DR     (UART0_BASE+0x00)
#define UART0_RSRECR (UART0_BASE+0x04)
#define UART0_FR     (UART0_BASE+0x18)
#define UART0_ILPR   (UART0_BASE+0x20)
#define UART0_IBRD   (UART0_BASE+0x24)
#define UART0_FBRD   (UART0_BASE+0x28)
#define UART0_LCRH   (UART0_BASE+0x2C)
#define UART0_CR     (UART0_BASE+0x30)
#define UART0_IFLS   (UART0_BASE+0x34)
#define UART0_IMSC   (UART0_BASE+0x38)
#define UART0_RIS    (UART0_BASE+0x3C)
#define UART0_MIS    (UART0_BASE+0x40)
#define UART0_ICR    (UART0_BASE+0x44)
#define UART0_DMACR  (UART0_BASE+0x48)
#define UART0_ITCR   (UART0_BASE+0x80)
#define UART0_ITIP   (UART0_BASE+0x84)
#define UART0_ITOP   (UART0_BASE+0x88)
#define UART0_TDR    (UART0_BASE+0x8C)

#define CS 0x20003000
#define CLO 0x20003004
#define C0 0x2000300C
#define C1 0x20003010
#define C2 0x20003014
#define C3 0x20003018

#define GPFSEL1  0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028
#define GPPUD       0x20200094
#define GPPUDCLK0   0x20200098

typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned short  U16;
typedef signed short    S16;
typedef unsigned int    U32;
typedef signed int      S32;

static inline void __str(U32 dst, U32 src)
{
    asm volatile("str %0, [%1]" : : "r" (src), "r" (dst) : "memory");
}

static inline U32 __ldr(U32 src)
{
    asm volatile("ldr %0, [%1]" : "=r" (src) :  "r" (src) : "memory");
    return src;
}

extern U32 sys_call(unsigned int, unsigned int, unsigned int, unsigned int);
extern U32* enterTask(unsigned int* sp);

void uart_init ( void )
{
    unsigned int ra;

    __str(UART0_CR,0);

    ra=__ldr(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=4<<12;    //alt0
    ra&=~(7<<15); //gpio15
    ra|=4<<15;    //alt0
    __str(GPFSEL1,ra);

    __str(GPPUD,0);
    __str(GPPUDCLK0,0);

    __str(UART0_ICR,0x7FF);
    __str(UART0_IBRD,1);
    __str(UART0_FBRD,40);
    __str(UART0_LCRH,0x70);
    __str(UART0_CR,0x301);
}

int uart_putc ( unsigned int c )
{
    while ((__ldr(UART0_FR) & 0x20) != 0)
    {
    }

    __str(UART0_DR, c);

    return 0;
}

int uart_getc ( void )
{
    while((__ldr(UART0_FR) & 0x10) != 0)
    {
    }

    return __ldr(UART0_DR);
}

char x2c(int c)
{
    if (c <= 9)
        return '0' + c;

    return 'a' + c - 10;
}

int printHex(unsigned int value)
{
    int byte;
    char chh, chl;
    unsigned char *str = (unsigned char*)&value;

    uart_putc('0');
    uart_putc('x');

    for (byte = 3; byte >= 0; byte--)
    {
        unsigned char c = str[byte];
        chh = x2c(c / 16);
        chl = x2c(c % 16);

        uart_putc(chh);
        uart_putc(chl);
    }

    uart_putc('\r');
    uart_putc('\n');

    return 0;
}

void task1()
{
    unsigned int speed = 1;
    while (1)
    {
        int test = 10;
        while (test-- > 0)
        {
            volatile int i;
            for (i = 0; i < 1000000* speed; i++)
            {
                asm volatile("nop");
            }

            __str(GPCLR0,1<<16);

            for (i = 0; i < 1000000* speed; i++)
            {
                asm volatile("nop");
            }
            __str(GPSET0, 1<<16);
        }

        speed = sys_call(0, 0, 0, 0);
        test = 10;
        while (test-- > 0)
        {
            volatile int i;
            for (i = 0; i < 100000* speed; i++)
            {
                asm volatile("nop");
            }

            __str(GPCLR0,1<<16);

            for (i = 0; i < 100000* speed; i++)
            {
                asm volatile("nop");
            }
            __str(GPSET0, 1<<16);
        }

        speed = sys_call(0, 0, 0, 0);
    }
}

int main(U32* pc)
{
    unsigned int ra = __ldr(GPFSEL1);
    unsigned int rx;

    ra&=~(7<<18);
    ra|=1<<18;

    __str(GPFSEL1, ra);
    __str(GPCLR0,1<<16);

    int* mem = 0;
    uart_init();

    uart_putc(27);
    uart_putc('[');
    uart_putc('2');
    uart_putc('J');
    uart_putc('\r');

    unsigned int task_stack[512];
    task_stack[496] = 0x10;
    task_stack[497] = 15;
    task_stack[498] = 0;
    task_stack[499] = 1;
    task_stack[500] = 2;
    task_stack[501] = 3;
    task_stack[502] = 4;
    task_stack[503] = 5;
    task_stack[504] = 6;
    task_stack[505] = 7;
    task_stack[506] = 8;
    task_stack[507] = 9;
    task_stack[508] = 10;
    task_stack[509] = 11;
    task_stack[510] = 12;
    task_stack[511] = (unsigned int)(&task1) + (U32)(pc);

    U32* sp = &(task_stack[496]);
    while(1)
    {
        char c = uart_getc();

        if (c >= 0)
        {
            if (c == 0x0D)
            {
                uart_putc(0x0A);
            }

            if (c == 'a')
            {
                int i;
                for (i = 0; i < 16; i++)
                {
                    printHex(*(sp + i));
                }

                sp = enterTask(sp);
                
                for (i = 0; i < 16; i++)
                {
                    printHex(*(sp + i));
                }
            }
            else
            {
                *(sp + 2) = c - '0';
            }

            uart_putc(c);
        }
    }

    return 0;
}
