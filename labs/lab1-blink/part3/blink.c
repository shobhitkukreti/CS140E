/*
 * blink for arbitrary pins.    
 * Implement:
 *	- gpio_set_output;
 *	- gpio_set_on;
 * 	- gpio_set_off.
 *
 *
 * - try deleting volatile.
 * - change makefile to use -O3
 * - get code to work by calling out to a set32 function to set the address.
 * - initialize a structure with all the fields.
 */

// see broadcomm documents for magic addresses.
#define GPIO_BASE 0x3F200000

// FSEL0 GPIO_BASE  // GPIO 0 - 9
// FSEL1 GPIO_BASE + 0x04  10 - 19
// FSEL2 GPIO_BASE + 0x08  20 - 29
// FSEL3 GPIO_BASE + 0xC   30 - 39
// FSEL4 GPIO_BASE + 0x10  40 - 49
// FSEL5 GPIO_BASE + 0x14  50 - 59

// GPIOSET0 GPIO_BASE + 1C
// GPIOSET1 GPIO_BASE + 20

// GPIOCLR0 GPIO_BASE + 28
// GPIOCLR1 GPIO_BASE + 2C

const static unsigned opVal = 0x01; // 3 bits

volatile unsigned *gpio_fsel1 = (volatile unsigned *)(GPIO_BASE + 0x04);
volatile unsigned *gpio_set0  = (volatile unsigned *)(GPIO_BASE + 0x1C);
volatile unsigned *gpio_clr0  = (volatile unsigned *)(GPIO_BASE + 0x28);

// XXX might need memory barriers.
void gpio_set_output(unsigned pin) {
	if( pin >=0 && pin <=9){ // GPIO 0 - 9
		unsigned regPin = pin;
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);

	}

	else if( pin >=10 && pin <=19){
		unsigned regPin = pin%10;		
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0x04);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);
	}

	else if( pin >=20 && pin <=29){
		unsigned regPin = pin%10;		
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0x08);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);
	}

	else if( pin >=30 && pin <=39){
		unsigned regPin = pin%10;		
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0xC);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);
	}

	else if( pin >=40 && pin <=49){
		unsigned regPin = pin%10;		
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0x10);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);
	}

	else if( pin >=50 && pin <=59){
		unsigned regPin = pin%10;		
		volatile unsigned int *addr = (volatile unsigned int *)(GPIO_BASE + 0x14);
		unsigned value = opVal << regPin * 3;
		put32(addr, value);
	}
	


}

void gpio_set_on(unsigned pin) {
	// use gpio_set0
	if(pin >=0 && pin <=31){
		volatile unsigned *addr = (volatile unsigned int *)(GPIO_BASE + 0x1C);
		unsigned value = 1<<(pin%32);
		put32(addr, value);
	}
	else if(pin >=32 && pin <=59){
	volatile unsigned *addr = (volatile unsigned int *)(GPIO_BASE + 0x20);
	unsigned value = 1<<(pin%32);
	put32(addr, value);
	}

}
void gpio_set_off(unsigned pin) {
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

// countdown 'ticks' cycles; the asm probably isn't necessary.
static void delay(unsigned ticks) {
	while(ticks-- > 0)
		asm("add r1, r1, #0");
}

int notmain ( void ) {
	int led = 20;

  	gpio_set_output(led);
        while(1) {
                gpio_set_on(led);
                delay(2000000);
                gpio_set_off(led);
                delay(2000000);
        }
	return 0;
}
