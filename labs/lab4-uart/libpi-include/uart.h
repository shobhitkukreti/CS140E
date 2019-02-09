#ifndef UART_DEFS
#define UART_DEFS

#define GPIO_BASE 0x3F200000
#define GPPUD GPIO_BASE + 0x94
#define GPPUDCLK0 GPIO_BASE + 0x98
#define GPPUDCLK1 GPIO_BASE + 0x9C

#define AUX_PER_REG_BASE  0x3F215000
#define AUX_IRQ AUX_PER_REG_BASE

#define AUXENB (AUX_PER_REG_BASE + 0x04)
// 
#define AUX_MU_IO_REG (AUX_PER_REG_BASE + 0x40)

// Interrupt Enable Register
#define AUX_MU_IER_REG (AUX_PER_REG_BASE + 0x44)

// Interupt Status Register
#define AUX_MU_IIR_REG (AUX_PER_REG_BASE + 0x48)

// Access to Baud rate Register, Data SIze
#define AUX_MU_LCR_REG (AUX_PER_REG_BASE + 0x4C)
// For Controlling Modem Signals
#define AUX_MU_MCR_REG (AUX_PER_REG_BASE + 0x50)

// Shows Data Status, Transmitter Idle/Empty, Data Ready
#define AUX_MU_LSR_REG (AUX_PER_REG_BASE + 0x54)

//Modem Status
#define AUX_MU_MSR_REG (AUX_PER_REG_BASE + 0x58)

// Mini Uart Control register
#define AUX_MU_CNTL_REG (AUX_PER_REG_BASE + 0x60)

// Mini Uart Status Register
#define AUX_MU_STAT_REG (AUX_PER_REG_BASE + 0x64)

// Baud Rate Register
#define AUX_MU_BAUD_REG (AUX_PER_REG_BASE + 0x68)



void uart_init ( void );
int uart_getc ( void );
void uart_putc ( unsigned int c );
#endif
