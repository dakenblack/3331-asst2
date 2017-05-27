#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "udp.h"

int main(){
  int clientSocket, nBytes;
  char buffer[1024] = "Ssdsdsfgsdgsdg";

  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
  nBytes = strlen(buffer);

  struct sockaddr_in addr = getAddr(7891);
  sendto(clientSocket,buffer,nBytes,MSG_NOSIGNAL,(struct sockaddr*)&addr,(socklen_t)sizeof(addr));
  return 0;
}
