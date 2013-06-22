// ************************************************************************
// *                                                                      *
// * Copyright (C) 2013 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#include <stdio.h>
#include <string.h>

#include "socket_support.h"


////////////////////////////////////////////////////////////////

static void print_bytes64(uint64_t data)
{
  union {
    uint64_t w;
    uint8_t  b[8];
  }u;

  u.w = data;

  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n",
	 u.b[0], u.b[1], u.b[2], u.b[3],
	 u.b[4], u.b[5], u.b[6], u.b[7]);
}

////////////////////////////////////////////////////////////////

static void print_bytes32(uint32_t data)
{
  union {
    uint32_t w;
    uint8_t  b[4];
  }u;

  u.w = data;

  printf("%02x %02x %02x %02x\n",
	 u.b[0], u.b[1], u.b[2], u.b[3]);
}

////////////////////////////////////////////////////////////////

int main(void)
{
  long rc;

  ///////////////////////////////////////////////////
  //         Test of changing byte order
  ///////////////////////////////////////////////////
  uint64_t data64 = 0x0102030405060708;
  printf("u64 (host):0x%016lx\n", data64);
  print_bytes64(data64);

  data64 = hton64(data64);
  printf("u64 (net):0x%016lx\n", data64);
  print_bytes64(data64);

  data64 = ntoh64(data64);
  printf("u64 (host):0x%016lx\n", data64);
  print_bytes64(data64);

  printf("\n");

  data64 = 0xcafedeca01020304;
  printf("u64 (host):0x%016lx\n", data64);
  print_bytes64(data64);

  hton64(&data64);
  printf("u64 (net):0x%016lx\n", data64);
  print_bytes64(data64);

  ntoh64(&data64);
  printf("u64 (host):0x%016lx\n", data64);
  print_bytes64(data64);

  printf("\n");

  uint32_t data32 = 0xdeadbeef;
  printf("u32 (host):0x%08x\n", data32);
  print_bytes32(data32);

  hton32(&data32);
  printf("u32 (net):0x%08x\n", data32);
  print_bytes32(data32);

  ntoh32(&data32);
  printf("u32 (host):0x%08x\n", data32);
  print_bytes32(data32);

  printf("\n");

  ///////////////////////////////////////////////////
  //         Test of address transformation
  ///////////////////////////////////////////////////

  uint32_t net_addr;
  char ip_address[20];  

  to_net_address("192.168.100.17", &net_addr);
  printf("Net-address (classic)  : 0x%08x\n", net_addr);

  to_net_address(ANY_IP_ADDRESS, &net_addr);
  printf("Net-address (any)      : 0x%08x\n", net_addr);

  to_net_address(LOOPBACK_IP_ADDRESS, &net_addr);
  printf("Net-address (loopback) : 0x%08x\n", net_addr);

  to_net_address(BROADCAST_IP_ADDRESS, &net_addr);
  printf("Net-address (broadcast): 0x%08x\n", net_addr);

  net_addr = 0xc0a86411; // 192.168.100.17
  to_ip_address(net_addr, ip_address, 20);
  printf("IP-address (classic)  : %s\n", ip_address);

  net_addr = 0x0; // 0.0.0.0
  to_ip_address(net_addr, ip_address, 20);
  printf("IP-address (any)      : %s\n", ip_address);

  net_addr = 0x7f000001; // 127.0.0.1
  to_ip_address(net_addr, ip_address, 20);
  printf("IP-address (loopback) : %s\n", ip_address);

  net_addr = 0xffffffff; // 255.255.255.255
  to_ip_address(net_addr, ip_address, 20);
  printf("IP-address (broadcast): %s\n", ip_address);

  printf("\n");

  ///////////////////////////////////////////////////
  //         Test of resolving hostname
  ///////////////////////////////////////////////////

  resolve_element resolve_list[10];
  unsigned actual;

  bzero(resolve_list, sizeof(resolve_list));
  rc = resolve_hostname("www.google.com", 80, resolve_list, 10, &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("*** ERROR (resolve_hostname)");
    return 1;
  }
  for (unsigned i=0; i < actual; i++) {
    to_ip_address(resolve_list[i].net_addr, ip_address, 20);
    printf("Net-addr=0x%08x (%s),\tprotocol=%04d, socktype=%04d\n",
	   resolve_list[i].net_addr,
	   ip_address,
	   resolve_list[i].protocol,
	   resolve_list[i].socktype);
  }

  printf("\n");

  ///////////////////////////////////////////////////
  //         Test of connected UDP socket
  ///////////////////////////////////////////////////

  int udp_socket;
  socket_address udp_local_sa;
  socket_address udp_dest_sa;

  rc = create_udp_socket(&udp_socket);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("*** ERROR (create_udp_socket)");
    return 1;
  }

  to_net_address(ANY_IP_ADDRESS, &udp_local_sa.net_addr);
  udp_local_sa.port = 14000;

  rc = bind_socket(udp_socket, udp_local_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("*** ERROR (bind_socket)");
    return 1;
  }

  to_net_address("192.168.0.1", &udp_dest_sa.net_addr);
  udp_dest_sa.port = 25000;

  rc = connect_socket(udp_socket, udp_dest_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("*** ERROR (connect_socket)");
    return 1;
  }

  bzero(&udp_local_sa, sizeof(udp_local_sa));
  bzero(&udp_dest_sa, sizeof(udp_dest_sa));

  get_socket_local_address(udp_socket, &udp_local_sa);
  get_socket_peer_address(udp_socket, &udp_dest_sa);

  to_ip_address(udp_local_sa.net_addr, ip_address, 20);
  printf("(UDP) Local IP : %s, port = %d\n", ip_address, udp_local_sa.port);

  to_ip_address(udp_dest_sa.net_addr, ip_address, 20);
  printf("(UDP) Dest IP  : %s, port = %d\n", ip_address, udp_dest_sa.port);

  rc = close_socket(udp_socket);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("*** ERROR (close_socket)");
    return 1;
  }

  return 0;
}
