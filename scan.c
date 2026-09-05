#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "sockaddr.h"
#include <netinet/in.h>
#include <time.h>


int main(){

	struct sockaddr_local sockaddr_in;
	struct sockaddr_local sockaddr_out;
  	struct timespec timer;

	int socket_fd = syscall(41,2,SOCK_DGRAM | SOCK_NONBLOCK,1);

	if(socket_fd < 0){
		int err = errno;
		printf("[ERROR]:%s\n" , strerror(errno)); 
		return 1;
	}


	char buffer[] = {
	   0x08 , 0x00 , 0x00, 0x00,
	  0x00 , 0xff , 0x00 , 0x01, 
	  'H' , 'E' , 'L' , 'L' , 'O',
	};

	uint8_t b_out = sizeof(buffer) % 2;

	uint16_t *ptr = (uint16_t *)&buffer;
	uint16_t counter = 0;

	for(int i =0; i < sizeof(buffer) / 2; i++){
		counter += *ptr;
		ptr++;
	}

	if(b_out != 0){
		counter += *(uint8_t *)ptr;
	}

	int checksum =~counter; 
	buffer[2] = (checksum >> 8) & 0xFF;
	buffer[3] = checksum & 0xFF;

	sockaddr_in.sin_family = 2;
	sockaddr_in.sin_port = 0x0000;
	sockaddr_in.sin_addr = (1 << 24) | (1 << 16) | (168 << 8) | 192;

	char content[120];
	
	uint8_t *add_ptr = (uint8_t *)&sockaddr_in.sin_addr;
        uint8_t *add_out = (uint8_t *)&sockaddr_out.sin_addr;

	uint16_t *id = (uint16_t *)&content[4];
	int32_t addrlen = sizeof(sockaddr_in);

	timer.tv_sec = 0;
	timer.tv_nsec = 500000000L;
	
   for(int i = 1; i < 255; i++){
      printf("scaning : %d.%d.%d.%d\n" , add_ptr[0] , add_ptr[1],add_ptr[2], add_ptr[3] );
      
      for(int count = 0; count < 5; count++){
	  memset(&content,0,120);
	  int sys_sendto = syscall(44,socket_fd,&buffer,sizeof(buffer),0,&sockaddr_in, sizeof(sockaddr_in));
	 

	  if(sys_sendto < 0){
	     int err = errno;
	     printf("Erro to send packet [ERROR]: %s\n" , strerror(err) );
	     return 1;
	   }

          nanosleep(&timer,NULL);
          int  recv = syscall(45,socket_fd,&content,120,0,&sockaddr_out,&addrlen);
	 
	  
	  if(*id > 0 && sockaddr_out.sin_addr == sockaddr_in.sin_addr){
	   printf("\033[32m%d.%d.%d.%d is up\033[0m\n" , add_out[0] , add_out[1] ,add_out[2] , add_out[3]);
	   count = 5;
 	  }
      }
      add_ptr[3] += 1;
  }
	
	syscall(3,socket_fd);
	return 0;
}
