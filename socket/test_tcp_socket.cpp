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
#include <strings.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "socket_support.h"

// Server definitions
#define SERVER_PORT 14000

// Client definitions
#define CLIENT_PORT 13000

// Data definitions
#define NR_DATA_ITEMS  250000
static uint32_t g_data[NR_DATA_ITEMS]; // Approx. 1 MB


////////////////////////////////////////////////////////////////

int do_server(void)
{
  long rc;
  int server_sd;
  int client_sd;
  socket_address server_sa;
  socket_address client_sa;
  char ip_address[20];
  unsigned actual;
  char message[] = "Server ready to close";

  // Create server socket
  rc = create_tcp_socket(&server_sd);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: create_tcp_socket\n");
    return 1;
  }
  set_opt_reuse_addr(server_sd, true);

  // Bind
  to_net_address(ANY_IP_ADDRESS, &server_sa.net_addr);
  server_sa.port = SERVER_PORT;

  rc = bind_socket(server_sd, server_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: bind_socket\n");
    close_socket(server_sd);
    return 1;
  }

  // Listen
  rc = listen_socket(server_sd, 1);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: listen_socket\n");
    close_socket(server_sd);
    return 1;
  }

  // Wait for client to connect
  rc = accept_socket(server_sd, &client_sd, &client_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: accept_socket\n");
    close_socket(server_sd);
    return 1;
  }
  to_ip_address(client_sa.net_addr, ip_address, 20);
  printf("Client connected from %s@%d\n",
	 ip_address, client_sa.port);
  
  // Wait for data
  rc = recv_socket(client_sd,
		   (void *)g_data, NR_DATA_ITEMS * sizeof(uint32_t),
		   true,  // Receive all
		   false, // No peek
		   &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: recv_socket\n");
    close_socket(server_sd);
    close_socket(client_sd);
    return 1;
  }
  printf("Received %d bytes\n", actual);

  // Check data
  for (unsigned i=0; i < NR_DATA_ITEMS; i++) {
    ntoh32(&g_data[i]);
    if (g_data[i] != i) {
      printf("ERROR: Unexpected data g_data[%d] == 0x%08x\n", i, g_data[i]);
      break;
    }
  }

  // Send response
  rc = send_socket(client_sd,
		   (const void *)message, sizeof(message),
		   false, // Simple send
		   &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: send_socket\n");
    close_socket(server_sd);
    close_socket(client_sd);
    return 1;
  }
  printf("Sent %d bytes\n", actual);

  // Wait for client to close connection
  rc = recv_socket(client_sd,
		   (void *)g_data, NR_DATA_ITEMS * sizeof(uint32_t),
		   false,  // Simple receive
		   false,  // No peek
		   &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: recv_socket\n");
  }
  printf("Received %d bytes\n", actual);
  if (actual == 0) {
    printf("Client closed connection\n");
  }
  
  close_socket(server_sd);
  close_socket(client_sd);

  return 0;
}

////////////////////////////////////////////////////////////////

int do_client(void)
{
  long rc;
  int client_sd;
  socket_address server_sa;
  socket_address client_sa;
  char ip_address[20];
  unsigned actual;
  char message[50];

  // Create socket
  rc = create_tcp_socket(&client_sd);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: create_tcp_socket\n");
    return 1;
  }
  set_opt_reuse_addr(client_sd, true);

  // Bind
  to_net_address(LOOPBACK_IP_ADDRESS, &client_sa.net_addr);
  client_sa.port = CLIENT_PORT;

  rc = bind_socket(client_sd, client_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: bind_socket\n");
    close_socket(client_sd);
    return 1;
  }

  // Connect to server
  to_net_address(LOOPBACK_IP_ADDRESS, &server_sa.net_addr);
  server_sa.port = SERVER_PORT;
  
  rc = connect_socket(client_sd, server_sa);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: connect_socket\n");
    close_socket(client_sd);
    return 1;
  }

  // Check server address
  bzero(&server_sa, sizeof(server_sa));
  get_socket_peer_address(client_sd, &server_sa);
  to_ip_address(server_sa.net_addr, ip_address, 20);
  printf("Connected to server at %s@%d\n",
	 ip_address, server_sa.port);  

  // Prepare data
  for (unsigned i=0; i < NR_DATA_ITEMS; i++) {
    g_data[i] = htonl(i);
  }

  // Send data to server
  rc = send_socket(client_sd,
		   (const void *)g_data, NR_DATA_ITEMS * sizeof(uint32_t),
		   true, // Send all
		   &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: send_socket\n");
    close_socket(client_sd);
    return 1;
  }
  printf("Sent %d bytes\n", actual);

  // Wait for response
  rc = recv_socket(client_sd,
		   (void *)message, 50,
		   false,  // Simple receive
		   false,  // No peek
		   &actual);
  if (rc != SOCKET_SUPPORT_SUCCESS) {
    printf("ERROR: recv_socket\n");
    close_socket(client_sd);
    return 1;
  }
  printf("Received %d bytes : %s\n", actual, message);

  // Close connection
  printf("Close connection to server\n");
  close_socket(client_sd);

  return 0;
}

////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("usage: %s <server|client>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "server") == 0) {
    return do_server();
  }
  if (strcmp(argv[1], "client") == 0) {
    return do_client();
  }

  printf("usage: %s <server|client>\n", argv[0]);
  return 1;
}
