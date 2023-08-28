#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h> 

#include "common.h"
#include "server.h"

#define STATE_UNINITIALIZE  0
#define STATE_INITIALIZED   1
#define STATE_BINDED        2
#define STATE_LISTENED      3
#define STATE_ACCEPTED      4
#define STATE_READ          5
#define STATE_WRITTEN       6

using namespace std;
int gCurrentStatus = STATE_UNINITIALIZE;

int start_server_with_web(char *argNetIf, int portNo) {
 int create_socket,new_socket,fd;
    int n;
    socklen_t addrlen;
    struct sockaddr_in address;
    if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) > 0)
        cout<<"The socket was created\n";
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argNetIf);//INADDR_ANY;inet_addr("localhost");
    address.sin_port = htons(portNo);
    cout<<"working \n";
    if (bind(create_socket,(struct sockaddr *)&address,sizeof(address)) == 0)
        cout<<"Binding Socket\n";
    while(1){
        listen(create_socket,3);
        addrlen = sizeof(struct sockaddr_in);
        cout<<"*************************\n";
        new_socket = accept(create_socket,(struct sockaddr *)&address,&addrlen);
        cout<<"*************************\n";
        char dobj[1024];
        if (new_socket > 0){
            n = read(new_socket, dobj, 1023);
            string query(dobj);
            string strl(query);
            string s, d;
            istringstream iss(strl);
            iss >> s;
            iss >> d;
            const char* source = s.c_str();
            const char* dest = d.c_str();
            // printf("Here is the message: %s %s\n",source,dest);
            printf("Here is the message: %s\n", dobj);
            n = write(new_socket, "I got your message", 18);
        }
    }
    close(new_socket);
    return close(create_socket);
    
/*
    int create_socket, new_socket;
    socklen_t addrlen;
    struct sockaddr_in address;
    
    char sendbuff[BUF_SIZE]; //buffer to send data
    char recvbuff[BUF_SIZE]; //buffer to read data
    struct sockaddr_in server_addr, client_addr;
    int n, len;
    
    memset(&server_addr, '0', sizeof(server_addr));
    memset(&client_addr, '0', sizeof(client_addr));
    memset(sendbuff, '0', BUF_SIZE);
    
    
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created\n");
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argNetIf);//INADDR_ANY;inet_addr("localhost");
    address.sin_port = htons(portNo);
    
    
    if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("ERROR on binding \n");
        return STATUS_ERROR;
    }    
    
    printf("The socket was binded. \n");
    
    //while(1){
        listen(create_socket, 5);
        addrlen = sizeof(struct sockaddr_in);
        new_socket = accept(create_socket, (struct sockaddr *)&address, &addrlen);
        
        if (new_socket > 0){
            printf("The socket was accepted. \n");
            memset(recvbuff, '0', BUF_SIZE);
            n = recv(new_socket, recvbuff, BUF_SIZE-1, MSG_WAITALL);
            //string query(dobj);
            //string strl(query);
            //string s, d;
            //istringstream iss(strl);
            //iss >> s;
            //iss >> d;
            //const char* source = s.c_str();
            //const char* dest = d.c_str();
            printf("Receive message: %s\n", recvbuff);
            strcpy(sendbuff, "Server response: Has got message.\n");
            // n = write(newsockfd, sendbuff, sizeof(sendbuff));
            n = write(new_socket, sendbuff, strlen(sendbuff));
        }
    //}
    close(new_socket);
    return close(create_socket);
    
*/
}

int start_server(char *argNetIf, int portNo) {
    int sockfd;
    int newsockfd;
    struct ifreq ifr;
    int isInputByIp = 0;
    FILE *fptr;
    
    char sendbuff[BUF_SIZE]; //buffer to send data
    char recvbuff[BUF_SIZE]; //buffer to read data
    char status[BUF_SIZE]; //buffer to status data
    char clientinfo[BUF_SIZE]; //buffer to client data
    struct sockaddr_in server_addr, client_addr;
    int n, len;
    
    memset(&server_addr, '0', sizeof(server_addr));
    memset(&client_addr, '0', sizeof(client_addr));
    memset(sendbuff, '0', BUF_SIZE);

    int isExitLoop = 0;
    while(1) {
        if(isExitLoop == 1) {
            break;
        }
        
        switch(gCurrentStatus) {
            case STATE_UNINITIALIZE:
                // printf("STATE_UNINITIALIZE. \n");
                //Create socket non-blocking
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) {
                    printf("ERROR opening socket \n");
                    gCurrentStatus = STATE_UNINITIALIZE;
                    continue;
                }
                
                // Get ipv4 address
                ifr.ifr_addr.sa_family = AF_INET;
                strncpy(ifr.ifr_name, argNetIf, IFNAMSIZ-1);
                if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
                {
                    // interface invalid, start by IP.
                    isInputByIp = 1;
                }    
                
                
                fcntl(sockfd, F_SETFL, O_NONBLOCK);
                gCurrentStatus = STATE_INITIALIZED;
                break;
            case STATE_INITIALIZED:
                // printf("STATE_INITIALIZED. \n");
                
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(portNo);      //port number
                
                if(isInputByIp == 1) {
                    if(inet_pton(AF_INET, argNetIf, &server_addr.sin_addr)<=0)
                    {
                        printf("\n inet_pton error occured\n");
                        return STATUS_ERROR;
                    }    
                }
                else {
                    server_addr.sin_addr.s_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr; //INADDR_ANY;  //ip server
                }

                //bind socket
                if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
                    printf("ERROR on binding \n");
                    gCurrentStatus = STATE_INITIALIZED;
                    continue;
                }
                
                gCurrentStatus = STATE_BINDED;
                break;
            case STATE_BINDED:
                // printf("STATE_BINDED. \n");
                //listening
                if(listen(sockfd,5) < 0) {   //5 backlog maximum                
                    printf("ERROR on listening \n");
                    gCurrentStatus = STATE_BINDED;
                    continue;
                }

                gCurrentStatus = STATE_LISTENED;
                break;
            case STATE_LISTENED:
                //printf("STATE_LISTENED. \n");
                len = sizeof(client_addr);
                // printf("Server is listening at ip %s and port %d\n", inet_ntoa(server_addr.sin_addr), portNo);
                newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t*)&len); // maybe non-blocking
                if (newsockfd < 0) 
                {
                    // printf("ERROR on accept \n");
                    close(newsockfd);
                    gCurrentStatus = STATE_LISTENED;
                    
                    // Store status
                    memset(status, 0, BUF_SIZE);                
                    strcpy(status, STATUS_DISCONNECTED);
                    write_status(status);
                    
                    continue;
                } 

                // Store status
                memset(status, 0, BUF_SIZE);                
                strcpy(status, STATUS_CONNECTED);
                write_status(status);
                
                char s[255]; // client address.
                inet_ntop(client_addr.sin_family,(struct sockaddr *)&client_addr,s, sizeof s);
                printf("Server: Got connection from %s\n", s);
                
                // Store client info
                memset(clientinfo, 0, BUF_SIZE);                
                strcpy(clientinfo, s);
                write_client_info(clientinfo);
                
                gCurrentStatus = STATE_ACCEPTED;
                break;
            case STATE_ACCEPTED:
                // printf("STATE_ACCEPTED. \n");
                memset(recvbuff, 0, BUF_SIZE);
                //read data from socket 
                n = read(newsockfd, recvbuff, BUF_SIZE-1);
                if (n <= 0) {
                    printf("Waiting for newsockfd=%d, n=%d \n", newsockfd, n);
                    
                    close(newsockfd);
                    // printf("ERROR on read \n");
                    gCurrentStatus = STATE_LISTENED;
                    
                    // Store status
                    memset(status, 0, BUF_SIZE);                
                    strcpy(status, STATUS_DISCONNECTED);
                    write_status(status);
                    
                    continue;                    
                }
                
                printf("Message from client: %s\n", recvbuff);
                
                //Write to message file
                FILE *fptr;
                fptr = fopen("./message.txt","w");

                if(fptr == NULL) {
                    //retry                    
                    fptr = fopen("./message.txt","w");
                }
                if(fptr == NULL)
                {
                  printf("Error!");   
                  exit(1);             
                }
                fprintf(fptr,"%s", recvbuff);
                fclose(fptr);
                
                gCurrentStatus = STATE_READ;
                break;
            case STATE_READ:
                // printf("STATE_READ. \n");
                //write data via socket
                strcpy(sendbuff, "Server response: Has got message.\n");
                // n = write(newsockfd, sendbuff, sizeof(sendbuff));
                n = write(newsockfd, sendbuff, strlen(sendbuff));
                if (n <= 0) {
                    close(newsockfd);
                    printf("ERROR on write \n");
                    gCurrentStatus = STATE_LISTENED;
                    
                    // Store status
                    memset(status, 0, BUF_SIZE);                
                    strcpy(status, STATUS_DISCONNECTED);
                    write_status(status);
                    
                    continue;                    
                }
                
                gCurrentStatus = STATE_WRITTEN;
                break;
            case STATE_WRITTEN:
                // printf("STATE_WRITTEN. \n");
                if (newsockfd < 0) 
                {
                    printf("ERROR on written \n");
                    gCurrentStatus = STATE_LISTENED;
                    
                    // Store status
                    memset(status, 0, BUF_SIZE);                
                    strcpy(status, STATUS_DISCONNECTED);
                    write_status(status);
                    
                    continue;
                } 
                    
                gCurrentStatus = STATE_ACCEPTED;
                break;
            default:
                // printf("default. \n");
                isExitLoop = 1;
                break;            
        }
        
        usleep(10);
    }

    close(newsockfd);
    close(sockfd);
    return STATUS_OK;
}
