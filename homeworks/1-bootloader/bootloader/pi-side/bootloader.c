/* 
 * very simple bootloader.  more robust than xmodem.   (that code seems to 
 * have bugs in terms of recovery with inopportune timeouts.)
 */

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

#include "libpi.small/rpi.h"

static void send_byte(unsigned char uc) {
	uart_putc(uc);
}
static unsigned char get_byte(void) { 
        return uart_getc();
}

static unsigned get_uint(void) {
	unsigned u = get_byte();
        u |= get_byte() << 8;
        u |= get_byte() << 16;
        u |= get_byte() << 24;
	return u;
}
static void put_uint(unsigned u) {
        send_byte((u >> 0)  & 0xff);
        send_byte((u >> 8)  & 0xff);
        send_byte((u >> 16) & 0xff);
        send_byte((u >> 24) & 0xff);
}

static void die(int code) {
        put_uint(code);
        reboot();
}

//  bootloader:
//	1. wait for SOH, size, cksum from unix side.
//	2. echo SOH, checksum(size), cksum back.
// 	3. wait for ACK.
//	4. read the bytes, one at a time, copy them to ARMBASE.
//	5. verify checksum.
//	6. send ACK back.
//	7. wait 500ms 
//	8. jump to ARMBASE.
//

#define GPIO_BASE 0x3F200000

void static gpio_set_io(unsigned pin) {
		unsigned regPin = pin%10;
		unsigned addr = (GPIO_BASE + 0x08);
		unsigned value = 1 << regPin * 3;
		PUT32(addr, value);

// set GPIO 20 on
		addr = (GPIO_BASE + 0x1C);
		value = 1<<(pin%32);
		PUT32(addr, value);
}

void static gpio_set_io_off(unsigned pin) {
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

void notmain(void) {

//	gpio_set_io(20);
	delay_ms(2000);
	uart_init();

	// XXX: cs107e has this delay; doesn't seem to be required if 
	// you drain the uart.
	//delay_ms(500);
	unsigned soh =0;
	volatile unsigned *armbase = (volatile unsigned *)ARMBASE;
	do{
		soh = get_uint();
	}while(soh!=0x12345678); // wait for SOH

	unsigned fsize = get_uint();
	unsigned crc32_rx = get_uint();

	put_uint(soh);
	put_uint(fsize);
	put_uint(crc32_rx);
	int ufsize = fsize/4;
	int tmp = 0;
	while(tmp < ufsize)
	{
		armbase[tmp] = get_uint();
		tmp++;
	}
	unsigned eot = get_uint();
	if(eot !=EOT)
		put_uint(BAD_END);
	else
		put_uint(EOT);
	
//	gpio_set_io_off(20);
	unsigned check_crc32 = crc32(ARMBASE, fsize/4);
	
	if(check_crc32!=crc32_rx){
		put_uint(BAD_CKSUM);
	}
	else{
		put_uint(ACK);
	}

	delay_ms(1000);

	/* XXX put your bootloader implementation here XXX */
	// XXX: appears we need these delays or the unix side gets confused.
	// I believe it's b/c the code we call re-initializes the uart; could
	// disable that to make it a bit more clean.
	//delay_ms(500);

	// run what client sent.
        BRANCHTO(ARMBASE);
	// should not get back here, but just in case.
	reboot();
}
