#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include "utils.h"

//CHANGE THIS AFTER FINDING OUT THE CORRECT SERVER PORT
#define SERVER_PORT 21
#define RESPONSE_BUFFER_SIZE 1024
#define MAX_RESPONSE_SIZE 1024


int connect_to_server(char* server_address);

int read_server_response(int control_socket, char *buffer, size_t size);

int send_command(int control_socket, const char *command, const char *argument);

int interpret_response(int control_socket,int response);

int login_on_server(int control_socket, const char *username, const char *password);

int change_working_directory(int control_socket, const char *path);

int send_pasv_command(int control_socket, char *pasv_ip, int *port);

int send_retr_command(int control_socket, const char *filename);

int download_file(int data_socket, const char *filename);

int close_socket(int socket);
