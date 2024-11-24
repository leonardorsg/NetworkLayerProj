#include "aux.h"
#include "FTP.h"

/**      (C)2000-2021 FEUP
 *       tidy up some includes and parameters
 * */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], "ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }

    struct TCP_input input;
    if(get_input(&input, argv[1]) != 0){
        printf("Error getting input\n");
        return -1;
    }

    printf("User: %s\n", input.user);
    printf("Password: %s\n", input.password);
    printf("Host: %s\n", input.host);
    printf("Path: %s\n", input.path);
    printf("Filename: %s\n", input.filename);

    char ip[MAX_SIZE];
    if(getIP(input.host, ip) != 0){
        printf("Error getting IP\n");
        return -1;
    }

    printf("IP: %s\n", ip);

    return 0;
}


