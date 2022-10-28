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
  int i = 5; // compteur pour ne pas avoir une boucle infinie
  int ok;    // resultat de sendto si negative alors erreur de sendto

  int sockfd;
  struct sockaddr_in addr;
  char buffer[1024];
  socklen_t addr_size;
  int n;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[-]socket error");
    exit(1);
  }

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);

  // -----------------------------création socket de communication-------------------------------------------
  int sockcom;
  int comPort; //on n'a pas encore reçu sa valeur
  struct sockaddr_in comAddr;
  socklen_t comAddr_size;

  sockcom = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockcom < 0)
  {
    perror("[-]comm socket error");
    exit(1);
  }
  memset(&comAddr, '\0', sizeof(comAddr));
  comAddr.sin_family = AF_INET;
  comAddr.sin_addr.s_addr = inet_addr(ip);

  // ------------------------------ three way handshake-------------------------------------------
  bzero(buffer, 1024);
  strcpy(buffer, "SYN");
  sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
  printf("[+]Data send: %s\n", buffer);
  printf("it's working\n");

  bzero(buffer, 1024);
  addr_size = sizeof(addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addr_size);
  printf("[+]Data recv: %s\n", buffer);
  //strcpy(&buffer[0],&buffer[1]);
  char * token = strtok(buffer, " ");
  printf( " %s\n", token ); //printing each token
  token = strtok(NULL, " ");
  printf("%s\n", token);
  token = strtok(NULL, " ");
  printf("%s\n", token);
  comAddr.sin_port = htons(atoi(token)); //Check if it works
  printf("%s\n", buffer);
  printf("ok\n");
  if (strncmp(buffer, "SYN", strlen("SYN")) == 0) //buffer became SYN (it changes after strtok)
  {
    printf("syn ACK's working\n");

    bzero(buffer, 1024);
    strcpy(buffer, "ACK");
    sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("[+]Data send: %s\n", buffer);

    if (ok > 0)
    {
      printf("connection established\n");
    }
  }

 //------------------------------------COMMUNICATION-----------------------------------------
  while (i > 0)
  {
    bzero(buffer, 1024);
    strcpy(buffer, "soy el cliente");
    sendto(sockcom, buffer, 1024, 0, (struct sockaddr *)&comAddr, sizeof(comAddr));
    printf("[+]Data send: %s\n", buffer);

    bzero(buffer, 1024);
    comAddr_size = sizeof(comAddr);
    recvfrom(sockcom, buffer, 1024, 0, (struct sockaddr *)&comAddr, &comAddr_size);
    printf("[+]Data recv: %s\n", buffer);
    i--;
  }
  printf("WE'VE DONE IT !!!! \n");
  
/**/ //------------------------------------TRANSFERT DE FICHIERS-----------------------------------------
  void write_file(int sockcom, struct sockaddr_in comAddr){
    
    char* filename = "client.txt";
    int n;
    char buffer[1024];
    socklen_t Addr_size;

    // Creating a file.
    FILE* fp = fp = fopen(filename, "w");

    // Receiving the data and writing it into the file.
    while (1)
    {
      addr_size = sizeof(comAddr);
      n = recvfrom(sockcom, buffer, 1024, 0, (struct sockaddr*)&comAddr, &comAddr_size);

      if (strcmp(buffer, "END") == 0)
      {
        break;
      }

      printf("[RECEVING] Data: %s", buffer);
      fprintf(fp, "%s", buffer);
      bzero(buffer, 1024);
    }

    fclose(fp);
    return;
  }


  printf("[STARTING] UDP File Server started. \n");
  write_file(sockcom, comAddr);

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Closing the server.\n");

  //close(sockcom);
}