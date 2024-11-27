#include "FTP.h"

int connect_to_server(char* server_address){
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }
    /*connect to the server*/
    if (connect(sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }    

    return sockfd;
}

int read_server_response(int sockfd) {
    char response[RESPONSE_BUFFER_SIZE];
    memset(response, 0, RESPONSE_BUFFER_SIZE);

    ssize_t bytes_read = read(sockfd, response, RESPONSE_BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        perror("read()");
        return -1;
    }

    printf("Server Response:\n%s\n", response);

    // Check if the response starts with "220" (connection success code)
    if (strncmp(response, "220", 3) == 0) {
        printf("Connection established successfully.\n");
        return 0;
    } else {
        fprintf(stderr, "Connection failed. Server response:\n%s\n", response);
        return -1;
    }
}