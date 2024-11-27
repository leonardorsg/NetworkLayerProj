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


int read_server_response(int socket_fd, char *buffer, size_t size) {
    ssize_t bytes_read = read(socket_fd, buffer, size - 1);
    if (bytes_read < 0) {
        perror("read()");
        return -1;
    }

    buffer[bytes_read] = '\0'; 
    printf("Server response: %s\n", buffer);

    return atoi(buffer); 
}


int send_command(int socket_fd, const char *command, const char *argument) {
    char buffer[MAX_RESPONSE_SIZE];

    if (argument != NULL) 
        snprintf(buffer, sizeof(buffer), "%s %s\r\n", command, argument);
    else 
        snprintf(buffer, sizeof(buffer), "%s\r\n", command);

    if (write(socket_fd, buffer, strlen(buffer)) < 0) {
        perror("write()");
        return -1;
    }

    return 0;
}

int login_on_server(int socket_fd, const char *username, const char *password) {
    char response[MAX_RESPONSE_SIZE];

    // Send USER command
    if (send_command(socket_fd, "USER", username) < 0) 
        return -1;

    // Read server response
    if (read_server_response(socket_fd, response, sizeof(response)) < 0)
        return -1;
    
    // Check response code
    int code = atoi(response);
    if (code == 230) {
        printf("Login successful (no password needed).\n");
        return 0;
    } else if (code != 331) {
        printf("Unexpected response after USER: %s\n", response);
        return -1;
    }

    // Send PASS command, 331 means password is required
    if (send_command(socket_fd, "PASS", password) < 0) 
        return -1;

    // Read server response
    if (read_server_response(socket_fd, response, sizeof(response)) < 0) 
        return -1;

    // Check response code
    code = atoi(response);
    if (code == 230) {
        printf("Login successful.\n");
        return 0;
    } else {
        printf("Login failed: %s\n", response);
        return -1;
    }
}
