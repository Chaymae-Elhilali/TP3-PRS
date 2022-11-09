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

  char src[50], dest[50]; // pour pouvoir envoyer SYN ACK + port avec la methode strcat
  strcpy(src,  " 8282");// A FAIRE: si on a plusieurs client il faut générer des ports disponibles random
  strcpy(dest, "SYN ACK"); 

  // -----------------------------création socket de three way handshake-------------------------------------------

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
  server_addr.sin_addr.s_addr = INADDR_ANY;

  n = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (n < 0)
  {
    perror("[-]bind error");
    exit(1);
  }

  // -----------------------------création socket de communication-------------------------------------------
  int sockcom;
  int comPort= atoi(src);
  printf("comm port %d\n", comPort);
  struct sockaddr_in comAddr;
  socklen_t comAddr_size;
  int i; // vérifie le fctionnement du bind

  sockcom = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockcom < 0)
  {
    perror("[-]comm socket error");
    exit(1);
  }
  memset(&comAddr, '\0', sizeof(server_addr));
  comAddr.sin_family = AF_INET;
  comAddr.sin_port = htons(comPort);
  comAddr.sin_addr.s_addr = INADDR_ANY;

  i = bind(sockcom, (struct sockaddr *)&comAddr, sizeof(comAddr));
  if (i < 0)
  {
    perror("[-]bind error");
    exit(1);
  }

  // -----------------------------three way handshake-------------------------------------------
  bzero(buffer, 1024);
  addr_size = sizeof(client_addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
  printf("[+]Data recv: %s\n", buffer);
  if (strcmp(buffer, "SYN") == 0)
  {
    printf("sync's working\n");

    bzero(buffer, 1024);
    strcpy(buffer, strcat(dest, src)); 
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

 //------------------------------------COMMUNICATION-----------------------------------------
  int k = 5;
  while (k>0)
  {
    bzero(buffer, 1024);
    addr_size = sizeof(client_addr);
    recvfrom(sockcom, buffer, 1024, 0, (struct sockaddr *)&comAddr, &comAddr_size);
    printf("[+]Data recv: %s\n", buffer);

    bzero(buffer, 1024);
    strcpy(buffer, "Welcome to the UDP Server.");
    sendto(sockcom, buffer, 1024, 0, (struct sockaddr *)&comAddr, comAddr_size); // on peut dans ce cas utiliser la m structure car c'est une reponse à sa précédente utilisation
    printf("[+]Data sent: %s\n", buffer);;
    k--;
  }
/**/ //------------------------------------TRANSFERT DE FICHIERS-----------------------------------------
  FILE *fp;
  char *filename= "serveur.txt";
  void send_file_data(FILE *fp, int sockcom, struct sockaddr_in comAddr){
    int n;
    char buffer[1024];

    while(fgets(buffer, 1024, fp)!= NULL){ //fread
      printf("[sending] Data: %s", buffer);
      n = sendto(sockcom, (char*)buffer, 1024, 0, (struct sockaddr*)&comAddr, sizeof(comAddr)); // A FAIRE utiliser une nouvelle structure à la place de comAddr car deja utilisée ou bien la memset et y remettre les bonnes valeurs
      printf("n:%d\n", n);
      if (n == -1)
      {
        perror("[ERROR] sending data to the client.");
        exit(1);
      }
      bzero(buffer, 1024);
    }
    strcpy(buffer, "END");
    sendto(sockcom, buffer, 1024, 0, (struct sockaddr*)&comAddr, sizeof(comAddr));

    //fclose(fp);

  }

  fp= fopen(filename,"r");
  if (fp==NULL){
    perror("ERROR reading the file\n");
    exit(1);
  }
  send_file_data(fp,sockcom, comAddr);
  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Disconnecting from the server.\n");
  //close(sockcom);
  return 0;
}