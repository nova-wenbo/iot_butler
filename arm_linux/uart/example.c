#include "iot_tty.h"


int main(int argc,char **argv) 
{ 
	TTY_INFO *ptty; 
	int nbyte,idx; 
	unsigned char buff[9]; 

	ptty = readyTTY(0); 
	if(ptty == NULL) 
	{ 
		printf("readyTTY(0) error\n"); 
		return 1; 
	} 

	lockTTY(ptty); 
	if(setTTYSpeed(ptty,115200)>0) 
	{ 
		printf("setTTYSpeed() error\n"); 
		return -1; 
	} 
	if(setTTYParity(ptty,8,'N',1)>0) 
	{ 
		printf("setTTYParity() error\n"); 
		return -1; 
	} 
	idx = 0; 
	while(1) 
	{ 
		buff[0] = 0xFA; 
		sendnTTY(ptty,&buff[0],1); 
		nbyte = recvnTTY(ptty,buff,8); 
		buff[nbyte] = '\0';
		printf("%s\n",buff); 
	} 

	cleanTTY(ptty); 
	return 0;

} 

