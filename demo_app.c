#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parse_options.h"
#include "common.h"
#include "server.h"
#include "client.h"

char *argMode = NULL;
char *argNetIf = NULL;
char *argPortNo = NULL;
char *argWeb = NULL;
    
int print_manual() {
    printf("\n");
    printf("Usage: demo_app -m <server|client> -i <network_interface> -p <port_number> -w true|false\n");
    printf("\t-m: Server or client mode \n");
    printf("\t-i: Network interface or IP address. \n");
    printf("\t-p: Port number (Ex: 12345). \n");
    printf("\t-w: Web communication (Ex: true). \n");
    
    return STATUS_OK;
}

void free_resource() {

    // Free memory due to parse_options()...
    if(argMode != NULL) {
        free(argMode);
        argMode = NULL;
    }
    
    if(argNetIf != NULL) {
        free(argNetIf);
        argNetIf = NULL;
    }
    
    if(argPortNo != NULL) {
        free(argPortNo);
        argPortNo = NULL;
    }
    
    if(argWeb != NULL) {
        free(argPortNo);
        argPortNo = NULL;
    }
}

int main(int argc, char **argv) {
    // Get option argument from input, the function call malloc()
    // So will need to free memory them
    parse_options(argc, argv, &argMode, &argNetIf, &argPortNo, &argWeb);
    
	printf ("%s argMode = %s, argNetIf = %s, argPortNo = %s, argWeb = %s\n", __FUNCTION__, argMode, argNetIf, argPortNo, argWeb);
    
    // Check input argument
    if(argMode == NULL || argNetIf == NULL || argPortNo == NULL || argWeb == NULL) {
        printf("Illegal empty input!! \n");
        print_manual();
        return STATUS_ERROR;
    }
    
    // Check port number.
    int portNo = atoi(argPortNo);
    if(portNo == 0) {
        printf("Illegal port number input!! \n");
        print_manual();
        free_resource();
        return STATUS_ERROR;
    }
    
    
    // Check network interfaces.
    GetIFs();
    
    // Check mode.
    int bResult = STATUS_OK;
    if(strcmp(argMode, MODE_SERVER) == 0) {
        if(strcmp(argWeb, "true") == 0) {        
            bResult = start_server_with_web(argNetIf, portNo);
        }
        else {
            bResult = start_server(argNetIf, portNo);
        }
    }
    else if(strcmp(argMode, MODE_CLIENT) == 0) {
        bResult = start_client(argNetIf, portNo);
    }
    else {    
        printf("Illegal mode input!! \n");
        print_manual();
    }
    
    free_resource();
    
    return STATUS_OK;
}




