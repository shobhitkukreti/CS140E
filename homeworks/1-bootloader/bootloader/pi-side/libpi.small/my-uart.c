#include "rpi.h"
#include "uart.h"
#include "gpio.h"
//#include "mem-barrier.h"

//Ref:: https://docs.particle.io/datasheets/discontinued/raspberrypi-datasheet/
// use this if you need memory barriers.

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


void dev_barrier(void) {
	dmb();
	dsb();
}
const unsigned ALT0 = 2;

void gpio_pud_clk(){
	
    volatile unsigned delay=150;
    PUT32(GPPUD, 0);
	while(--delay) asm volatile("nop"); 
	PUT32(GPPUDCLK0, (1<<14 | 1<<15));
	delay=150;
	while(--delay) asm volatile("nop"); 
	PUT32(GPPUDCLK0, 0);	
}

void gpio_set_serial_func(unsigned pin) {
		unsigned regPin = pin%10;
		unsigned val=0;
        // GPFSEL1 for GPIO 14, 15   
		val = GET32((GPIO_BASE + 0x04));
        val &= (~(7<<regPin*3)) & val;
        val|= (ALT0 << (regPin * 3));
		PUT32( (GPIO_BASE + 0x04), val) ;

	}

void gpio_set_io(unsigned pin) {
		unsigned regPin = pin%10;
		unsigned addr = (GPIO_BASE + 0x08);
		unsigned value = 1 << regPin * 3;
		PUT32(addr, value);

// set GPIO 20 on
		addr = (GPIO_BASE + 0x1C);
		value = 1<<(pin%32);
		PUT32(addr, value);
}

void gpio_set_io_off(unsigned pin) {
	// use gpio_clr0
	if(pin >=0 && pin <=31){
		unsigned addr = (GPIO_BASE + 0x28);
		unsigned value = 1<<(pin%32);
		PUT32(addr, value);
	}
	else if(pin >=32 && pin <=59){
	unsigned addr = (GPIO_BASE + 0x2C);
	unsigned value = 1<<(pin%32);
	PUT32(addr, value);
	}
}


const unsigned baudRateReg = 270; //baud 115200
void uart_init(void) {
// disable uart
// GPIO14, 15 need to select ALT0 for TX and RX
PUT32(AUXENB, 0x01); // enable uart

PUT32((AUX_MU_CNTL_REG), 0);
PUT32(AUX_MU_LCR_REG, 0x03); // data size set to 8 bits
PUT32((AUX_MU_MCR_REG), 0);

PUT32((AUX_MU_IER_REG), 0);
PUT32((AUX_MU_IIR_REG), 0xC6);
PUT32(AUX_MU_BAUD_REG, baudRateReg);

gpio_set_serial_func(14);   //tx
gpio_set_serial_func(15);   //rx
gpio_pud_clk();
 
PUT32(AUX_MU_CNTL_REG, 0x3); //enable rx,tx

}

int uart_getc(void) {
	
	do {
		asm volatile("nop");
	} while(!(GET32(AUX_MU_LSR_REG) & 0x1));
	
	int byt = GET32(AUX_MU_IO_REG);
	return (byt & 0xff);
}
void uart_putc(unsigned c) {
	do { asm volatile("nop"); 
	} while(!(GET32(AUX_MU_LSR_REG)& 0x20));
	PUT32(AUX_MU_IO_REG, c);

}




