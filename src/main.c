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
// 10. Disconnect from socket  ✅

/*
PUBLICOS:
build/download ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz
build/download ftp://demo:password@test.rebex.net/readme.txt
build/download ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin

FEUP:
build/download ftp://rcom:rcom@ftp.netlab.fe.up.pt/README
build/download ftp://rcom:rcom@ftp.netlab.fe.up.pt/pipe.txt
build/download ftp://rcom:rcom@ftp.netlab.fe.up.pt/files/crab.mp4 

/debian/ls-lR.gz (about 15 MB)
/pub/parrot/iso/testing/Parrot-architect-5.3_amd64.iso

*/

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
    if (getIP(input.host, ip) != 0){
        printf("Error getting IP\n");
        return -1;
    }

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

    if (input.path != NULL && strlen(input.path) != 0) {
        if (change_working_directory(control_socket, input.path) != 0) {
            printf("Error changing working directory.\n");
            close(control_socket);
            return -1;
        }
    } 

    char pasv_ip[INET_ADDRSTRLEN];
    int port;

    if (send_pasv_command(control_socket, pasv_ip, &port) < 0) {
        printf("Failed to establish data connection.\n");
        close(control_socket);
        return -1;
    }
    
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket < 0) {
        perror("socket");
        close_socket(control_socket);
        return -1;
    }

    struct sockaddr_in data_addr;
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, pasv_ip, &data_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(data_socket);
        close(control_socket);
        return -1;
    }

    if (connect(data_socket, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        perror("connect");
        close(data_socket);
        close(control_socket);
        return -1;
    }

    printf("Data connection established successfully.\n");

    // TODO: may need to uncomment lines 144, 145, 147 and 155
    // char full_path[MAX_SIZE];
    // snprintf(full_path, sizeof(full_path), "%s/%s", input.path, input.filename);    

    // if (send_retr_command(control_socket, full_path) < 0){
    if (send_retr_command(control_socket, input.filename) < 0){
        printf("Failed to send RETR command.\n");
        close(data_socket);
        close(control_socket);
        return -1;
    }

    // if (download_file(data_socket, full_path) < 0) {
    if (download_file(data_socket, input.filename) < 0) {
        printf("Failed to download file.\n");
        close(data_socket);
        close(control_socket);
        return -1;
    }
    
    if (close(data_socket) < 0) {
        perror("Failed to close data socket.");
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

    printf("Downloaded successfully.\n");

    close_socket(control_socket);
    return 0;
}


