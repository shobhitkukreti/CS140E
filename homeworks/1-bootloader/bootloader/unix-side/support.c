#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "demand.h"
#include "../shared-code/simple-boot.h"
#include "support.h"

const static unsigned ADDR_MULTIPLE=4;
// read entire file into buffer.  return it, write totat bytes to <size>
unsigned char *read_file(int *size, const char *name) {
	//unimplemented();
struct stat fileInfo;
unsigned int fsize=0;
int paddedfileSize=0;
unsigned char *fileBuf=NULL;
int fd=-1;
int retBytes=0;

if( stat(name, &fileInfo)==-1)
	return NULL;


fsize = fileInfo.st_size;


if(fsize % ADDR_MULTIPLE ){
	paddedfileSize += fsize + (ADDR_MULTIPLE - (fsize % ADDR_MULTIPLE)); // multiple of 4
*size = paddedfileSize;
fileBuf = (unsigned char*) calloc(paddedfileSize, 1);
fsize = paddedfileSize;
}	
else{
  *size = fsize;
  fileBuf = (unsigned char*) calloc(fsize, 1);
  paddedfileSize = fsize;
}

fd=open(name, O_RDONLY ,0);
if(fd==-1)
	return NULL;
else{
	if((retBytes = read(fd, (void*)fileBuf, fsize))!=fsize)
		printf("Read retBytes: %u Against File Size %u\n ", retBytes, fsize);
}

unsigned char* tmp  = fileBuf;

for(int i=0; i < fsize; i++){
  printf("%02x ", tmp[i]);
}
printf("\n");

close(fd);


return fileBuf;
}

#define _SVID_SOURCE
#include <dirent.h>
const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
	"tty.SLAB_USB", // mac os
	0
};

int file_select(const struct dirent *entry)
{
	int ret = 0;
   
   if(strstr(entry->d_name, ttyusb_prefixes[0]) || 
   		strstr(entry->d_name, ttyusb_prefixes[1]) )
   		ret=1;

   	return ret;
}

// open the TTY-usb device:
//	- use <scandir> to find a device with a prefix given by ttyusb_prefixes
//	- returns an open fd to it
// 	- write the absolute path into <pathname> if it wasn't already
//	  given.
int open_tty(const char **portname) {
int fd = -1;
struct dirent **namelist=NULL;
           int n;
           if(*portname!=NULL)
         	  printf("portname :: %s\n", *portname);
           n = scandir("/dev", &namelist, file_select, alphasort);
           if (n == -1) {
               perror("scandir");
               exit(EXIT_FAILURE);
           }
           n--;
           while (n>-1) {
               printf("%s\n", namelist[n]->d_name);
               if(fd==-1)
               {
               		char filePath[256];
               		sprintf(filePath, "/dev/%s", namelist[n]->d_name);
               		fd = open(filePath, O_RDWR);
           			}
                // free remaining members of namelist
               if(namelist[n]!=NULL){
                     free(namelist[n]);
                    namelist[n]=NULL;
              n--;
            }
           }
         	free(namelist);
         	namelist=NULL;
         
	return fd;
}
