#include <stdint.h>

#ifndef SOCK_ADDR
#define SOCK_ADDR

struct sockaddr_local {
  uint16_t  sin_family;
  uint16_t  sin_port;
  uint32_t  sin_addr;
  uint8_t  __pad[8];
	};

#endif
