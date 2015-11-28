//
//  proxy_server.cpp
//  
//
//  Created by Huiye Liu on 11/12/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>

#include <pthread.h>
#include <string.h>

#include "tcp_client.h"

static const int MAXPENDING = 5; // Maximum outstanding connection requests
static const int BUFSIZE = 512;

void HandleTCPClient(int clntSocket)
{
  char buffer[BUFSIZE]; // Buffer for echo string
  
  // Receive message from client
  ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
  if (numBytesRcvd < 0)
    printf("recv() failed");
  
  // Send received string and receive again until end of stream
  while (numBytesRcvd > 0)
  { // 0 indicates end of stream
    // Echo message back to client
    ssize_t numBytesSent = send(clntSocket, buffer, numBytesRcvd, 0);
    if (numBytesSent < 0)
      printf("send() failed");
    else if (numBytesSent != numBytesRcvd)
      printf("send() sent unexpected number of bytes");
    
    // See if there is more data to receive
    numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0)
      printf("recv() failed");
  }
  
  close(clntSocket); // Close client socket
}

unsigned int server_p;
unsigned int client_p;

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    printf("Usage: proxy <client-port> <server-port> \n");
    return -1;
  }
  
  client_p = atoi(argv[1]);
  server_p = atoi(argv[2]);
  
  //create socket for incoming connections
  int servSock;
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
  {
    printf("socket() failed\n");
    return -1;
  }
  
  //construct local address address
  struct sockaddr_in addr_l;                    //local address
  memset(&addr_l, 0, sizeof(addr_l));       // Zero out structure
  addr_l.sin_family = AF_INET;                  // IPv4 address family
  addr_l.sin_port = htons(client_p);            // Local Port
  
  //bind to local address
  if(bind(servSock, (struct sockaddr*) &addr_l, sizeof(addr_l)) < 0)
  {
    printf("failed to bind socket\n");
    return -1;
  }
  
  // Mark the socket so it will listen for incoming connections
  if(listen(servSock, MAXPENDING) < 0)
  {
    printf("failed to listen on socket\n");
    return -1;
  }

  // Connect to the bank
  tcp_client c;
  c.conn("localhost", server_p);

  std::cout << "Proxy created successfully \nListening on port " << client_p << ", connected to bank on port " << server_p << std::endl;
  
  //loop forever
  while(1)
  {
    struct sockaddr_in clientAddr;
    socklen_t size = sizeof(clientAddr);    // client address length
    
    // Wait for a client to connect
    int clientSock = accept(servSock, (struct sockaddr *) &clientAddr, &size);
    if(clientSock < 0)	//bad client, skip it
    {
      printf("accept() failed");
      continue;
    }
    
    // clntSock is connected to a client!
    
    
    char clientName[INET_ADDRSTRLEN];                     // String to contain client address
    if (inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL)
      printf("Handling client %s/%d\n", clientName, ntohs(clientAddr.sin_port));
    else
      puts("Unable to get client address");
    
    HandleTCPClient(clientSock);
    
    //pthread_t thread;
  }
}














