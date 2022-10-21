#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{

  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  char *ip = "127.0.0.1";
  int port = atoi(argv[1]);
  char port2 [1024];
  port2="2244" // A FAIRE si on a plusieurs client il faut générer des ports disponibles 

  int sockfd;
  struct sockaddr_in server_addr, client_addr; // le server_addr sert pour le bind et est donc non necessaire dans le code client
  char buffer[1024];
  socklen_t addr_size;
  int n;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[-]socket error");
    exit(1);
  }

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]bind error");
    exit(1);
  }

  // three way handshake
  bzero(buffer, 1024);
  addr_size = sizeof(client_addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
  printf("[+]Data recv: %s\n", buffer);
  if (strcmp(buffer, "SYN") == 0)
  {
    printf("sync's working\n");

    bzero(buffer, 1024);
    strcpy(buffer, strcat("SYN ACK", port2)); // CEST LA QUE CA FOIRE
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, addr_size);
    printf("[+]Data sent: %s\n", buffer);

    bzero(buffer, 1024);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);
    if (strcmp(buffer, "ACK") == 0)
    {
      printf("3WH is working  HOORA\n");
      printf("connection established\n");
    }
  }
  else
  {
    printf("sync isn't working\n");
    exit(-1);
  }

  while (1)
  {
    bzero(buffer, 1024);
    addr_size = sizeof(client_addr);
    recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    printf("[+]Data recv: %s\n", buffer);

    bzero(buffer, 1024);
    strcpy(buffer, "Welcome to the UDP Server.");
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, addr_size);
    printf("[+]Data sent: %s\n", buffer);
  }
}