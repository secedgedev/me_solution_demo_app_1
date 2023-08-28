#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int GetIFs() {
    struct ifaddrs *ifaddr;
    int family;
    
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }
    
    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
			printf("%s\t", ifa->ifa_name);
			printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");
			char ap[100];
			const int family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
			getnameinfo(ifa->ifa_addr,family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
        }
    }
    freeifaddrs(ifaddr);
	return STATUS_OK;
}

int check_network_interface(char *netIf) {
	return STATUS_OK;
}

int write_status(const char status[]) {
    FILE *fptr;
    fptr = fopen(FILE_STATUS,"w");

    if(fptr == NULL) {
        //retry                    
        fptr = fopen(FILE_STATUS,"w");
    }
    if(fptr == NULL)
    {
      printf("Error!");   
      exit(1);             
    }
    
    fprintf(fptr,"%s", status);
    fclose(fptr);
    
    return STATUS_OK;
}

int write_client_info(const char clientinfo[]) {
    FILE *fptr;
    fptr = fopen(FILE_CLIENT,"w");

    if(fptr == NULL) {
        //retry                    
        fptr = fopen(FILE_CLIENT,"w");
    }
    if(fptr == NULL)
    {
      printf("Error!");   
      exit(1);             
    }
    
    fprintf(fptr,"%s", clientinfo);
    fclose(fptr);
    
    return STATUS_OK;
}
