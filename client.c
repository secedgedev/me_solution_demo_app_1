#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h> 

#include "common.h"
#include "client.h"

int start_client(char *argNetIf, int portNo) {
    int sockfd, n;
    struct sockaddr_in server_addr;
    struct ifreq ifr;
    int isInputByIp = 0;

    char sendbuff[BUF_SIZE];
    char recvbuff[BUF_SIZE];

   //Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) printf("ERROR opening socket");      
    

    // Get ipv4 address
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, argNetIf, IFNAMSIZ-1);
    if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
    {
        // interface invalid, start by IP.
        isInputByIp = 1;
    }    
    
    memset(&server_addr, '0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNo);
    
    if(isInputByIp == 1) {
        if(inet_pton(AF_INET, argNetIf, &server_addr.sin_addr)<=0)
        {
            printf("\n inet_pton error occured\n");
            return STATUS_ERROR;
        }    
    }
    else {
        server_addr.sin_addr.s_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    }

    if( connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return STATUS_ERROR;
    }
        
    while(1) {    
        printf("Please enter the message (Q or q to exit): ");        
        memset(sendbuff, 0, BUF_SIZE);
        fgets(sendbuff, BUF_SIZE-1, stdin);
        
        if(sendbuff[0] == 'Q' || sendbuff[0] == 'q') {        
            close(sockfd);
            return STATUS_OK;
        }
        
        n = write(sockfd, sendbuff, strlen(sendbuff));
        if (n < 0) printf("ERROR writing to socket");
        
        memset(recvbuff, 0, BUF_SIZE);
        n = read(sockfd, recvbuff, BUF_SIZE-1);
        if (n < 0) printf("ERROR reading from socket");
        printf("%s\n",recvbuff);
    }
   
    close(sockfd);
    return STATUS_OK;
}


