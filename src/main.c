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
// 6. Change working directory on server to the file_path that was passed by input✅
// 7. Send PASV command to get IP address and port for data socket✅
// 8. Send RETR command to begin file transfer through data socket✅
// 9. Download file✅
// 10. Disconnect from socket  

// nslookup netlab1.fe.up.pt
// Server:         10.255.255.254
// Address:        10.255.255.254#53

// Name:   netlab1.fe.up.pt
// Address: 192.168.109.136

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
    int control_socket = connect_to_server(ip);
    if (control_socket < 0){
        printf("Error connecting to server\n");
        return -1;
    }

    char response[MAX_RESPONSE_SIZE];
    int response_code = read_server_response(control_socket, response, sizeof(response));
    if (response_code < 0) {
        printf("Failed to establish a proper connection with the server.\n");
        close(control_socket);
        return -1;
    } 

    if (login_on_server(control_socket, input.user, input.password) != 0) {
        printf("Error logging in to server.\n");
        close(control_socket);
        return -1;
    }

    if (change_working_directory(control_socket, input.path) != 0) {
        printf("Error changing working directory.\n");
        close(control_socket);
        return -1;
    }

    char pasv_ip[INET_ADDRSTRLEN];
    int port;

    if (send_pasv_command(control_socket, pasv_ip, &port) < 0) {
        printf("Failed to establish data connection.\n");
        close(control_socket);
        return -1;
    }

    printf("Data connection info: IP=%s, Port=%d\n", pasv_ip, port);


    if(send_retr_command(control_socket, input.filename) < 0){
        printf("Failed to send RETR command.\n");
        close(control_socket);
        return -1;
    }

    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in data_addr;
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &data_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        perror("connect");
        close(data_socket);
        return -1;
    }

    printf("Data connection established successfully.\n");

    if (download_file(data_socket, input.filename) < 0) {
        printf("Failed to download file.\n");
        close(data_socket);
        close(control_socket);
        return -1;
    }

    char response_download[MAX_RESPONSE_SIZE];
    int response_code_download = read_server_response(control_socket, response_download, sizeof(response_download));
    if (response_code_download < 0) {
        printf("Failed to download.\n");
        close(control_socket);
        return -1;
    } 

    close_socket(control_socket);
    return 0;
}


