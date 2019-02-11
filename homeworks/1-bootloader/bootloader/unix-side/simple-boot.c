#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "demand.h"

#define __SIMPLE_IMPL__
#include "../shared-code/simple-boot.h"

static void send_byte(int fd, unsigned char b) {
	if(write(fd, &b, 1) < 0)
		panic("write failed in send_byte\n");
}
static unsigned char get_byte(int fd) {
	unsigned char b;
	int n;
	if((n = read(fd, &b, 1)) != 1)
		panic("read failed in get_byte: expected 1 byte, got %d\n",n);
	return b;
}

// NOTE: the other way to do is to assign these to a char array b and 
//	return *(unsigned)b
// however, the compiler doesn't have to align b to what unsigned 
// requires, so this can go awry.  easier to just do the simple way.
// we do with |= to force get_byte to get called in the right order 
// 	(get_byte(fd) | get_byte(fd) << 8 ...) 
// isn't guaranteed to be called in that order b/c | is not a seq point.
static unsigned get_uint(int fd) {
        unsigned u = get_byte(fd);
        u |= get_byte(fd) << 8;
        u |= get_byte(fd) << 16;
        u |= get_byte(fd) << 24;
        return u;
}

void put_uint(int fd, unsigned u) {
	// mask not necessary.
        send_byte(fd, (u >> 0)  & 0xff);
        send_byte(fd, (u >> 8)  & 0xff);
        send_byte(fd, (u >> 16) & 0xff);
        send_byte(fd, (u >> 24) & 0xff);
}

// simple utility function to check that a u32 read from the 
// file descriptor matches <v>.
void expect(const char *msg, int fd, unsigned v) {
	unsigned x = get_uint(fd);
	if(x != v) 
		panic("%s: expected %x, got %x\n", msg, v,x);
}

// unix-side bootloader: send the bytes, using the protocol.
// read/write using put_uint() get_unint().

void delay50(){
	unsigned volatile delay=0;
	for(int i=0; i < 8000000; i++){
		delay++;
	}
}

void simple_boot(int fd, const unsigned char * buf, unsigned n) { 
	//unimplemented();
	
	printf("Fsize Simple Boot: %d\n", n);
	put_uint(fd,SOH);
	put_uint(fd,n);
	
	printf("SOH\n");

	unsigned crc32_tx = crc32(buf, n/4);
	put_uint(fd,crc32_tx);
	printf("CRC32 TX : %u \n", crc32_tx);	

	if( (get_uint(fd)==SOH) && (get_uint(fd)==n) && (get_uint(fd)==crc32_tx))
		printf("Received Checksum is Valid!\n");
	else{
		printf("Received Checksum is InValid!\n");
		close(fd);
		exit(-1);
	}

	unsigned *ubuf = (unsigned *)&buf[0];
	for(int i=0; i < n/4; i++){
		put_uint(fd, ubuf[i]);
		delay50();
	}
	
	printf("File TX Done\n");	
	put_uint(fd, EOT);
	delay50();
	if(get_uint(fd)==EOT){
		printf("EOT RX from BootLoader\n");
	}
	else{
		printf("EOT RX Not Received\n");
		close(fd);
		exit(-1);	
	}

	if(get_uint(fd)==ACK){
		printf("ACK from BootLoader\n");
	}
	else{
		printf("NAK Received\n");
		close(fd);
		exit(-1);	
	}
	usleep(1000000);

}
