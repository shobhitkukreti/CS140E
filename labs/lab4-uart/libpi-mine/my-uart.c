#include "rpi.h"
#include "uart.h"
#include "gpio.h"
#include "mem-barrier.h"

//Ref:: https://docs.particle.io/datasheets/discontinued/raspberrypi-datasheet/
// use this if you need memory barriers.

void dev_barrier(void) {
	dmb();
	dsb();
}
const unsigned ALT0 = 2;

void gpio_pud_clk(){
	
    volatile unsigned delay=150;
    put32((volatile unsigned *)GPPUD, 0);
	while(--delay) asm volatile("nop"); 
	put32((volatile unsigned *)GPPUDCLK0, (1<<14 | 1<<15));
	delay=150;
	while(--delay) asm volatile("nop"); 
	put32((volatile unsigned *)GPPUDCLK0, 0);	
}

void gpio_set_serial_func(unsigned pin) {
		unsigned regPin = pin%10;
		unsigned val=0;
        // GPFSEL1 for GPIO 14, 15   
		val = get32( (volatile unsigned int *)(GPIO_BASE + 0x04) );
        val &= (~(7<<regPin*3)) & val;
        val|= (ALT0 << (regPin * 3));
		put32( (volatile unsigned int *)(GPIO_BASE + 0x04), val) ;

	}

void gpio_set_io(unsigned pin) {
		unsigned regPin = pin%10;
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0x08);
		unsigned value = 1 << regPin * 3;
		put32(addr, value);

// set GPIO 20 on
		addr = (volatile unsigned int *)(GPIO_BASE + 0x1C);
		value = 1<<(pin%32);
		put32(addr, value);
}

void gpio_set_io_off(unsigned pin) {
	// use gpio_clr0
	if(pin >=0 && pin <=31){
		volatile unsigned *addr = (volatile unsigned int *)(GPIO_BASE + 0x28);
		unsigned value = 1<<(pin%32);
		put32(addr, value);
	}
	else if(pin >=32 && pin <=59){
	volatile unsigned *addr = (volatile unsigned int *)(GPIO_BASE + 0x2C);
	unsigned value = 1<<(pin%32);
	put32(addr, value);
	}
}


const unsigned baudRateReg = 270; //baud 115200
void uart_init(void) {
// disable uart
// GPIO14, 15 need to select ALT0 for TX and RX
put32((volatile unsigned*)AUXENB, 0x01); // enable uart

put32((volatile unsigned*)(AUX_MU_CNTL_REG), 0);
put32((volatile unsigned*)AUX_MU_LCR_REG, 0x03); // data size set to 8 bits
put32((volatile unsigned*)(AUX_MU_MCR_REG), 0);

put32((volatile unsigned*)(AUX_MU_IER_REG), 0);
put32((volatile unsigned*)(AUX_MU_IIR_REG), 0xC6);
put32((volatile unsigned*)AUX_MU_BAUD_REG, baudRateReg);

gpio_set_serial_func(14);   //tx
gpio_set_serial_func(15);   //rx
gpio_pud_clk();
 
put32((volatile unsigned*)AUX_MU_CNTL_REG, 0x3); //enable rx,tx

}

int uart_getc(void) {
	
	do {
		asm volatile("nop");
	} while(!(get32((volatile unsigned *)AUX_MU_LSR_REG) & 0x1));
	
	int byt = get32((volatile unsigned*)AUX_MU_IO_REG);
	return (byt & 0xff);
}
void uart_putc(unsigned c) {
	for(int i=0; i < 2000; i++){
			gpio_set_io(20);
	}
	do { asm volatile("nop"); 
	} while(!(get32((volatile unsigned *)AUX_MU_LSR_REG)& 0x20));
	
	for(int i=0; i < 2000; i++){
	gpio_set_io_off(20);
	}
	put32((volatile unsigned*)AUX_MU_IO_REG, c);

}




