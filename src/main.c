#include "utils.h"
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


//   RFC 959 Protocol

// 1. Receive from input "ftp://[<user>:<password>@]<host>/<url-path>"✅
// 2. get IP address from host✅
// 3. Create and connect a new socket to the server✅
// 4. Receive confirmation from server that socket is connected✅
// 5. Login on server✅
// 6. Change working directory on server to the file_path that was passed by input
// 7. Send PASV command to get IP address and port for data socket
// 8. Send RETR command to begin file transfer through data socket
// 9. Download file
// 10. Disconnect from socket  

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s %s\n", argv[0], "ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }

    struct TCP_input input;
    if (get_input(&input, argv[1]) != 0){
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

    //Creation of socket to connect to server
    int socketfd = connect_to_server(ip);
    if (socketfd < 0){
        printf("Error connecting to server\n");
        return -1;
    }

    char response[MAX_RESPONSE_SIZE];
    int response_code = read_server_response(socketfd, response, sizeof(response));
    if (response_code < 0) {
        printf("Failed to establish a proper connection with the server.\n");
        close(socketfd);
        return -1;
    } 
    
    if (login_on_server(socketfd, input.user, input.password) != 0) {
        printf("Error logging in to server.\n");
        close(socketfd);
        return -1;
    }

    return 0;
}


