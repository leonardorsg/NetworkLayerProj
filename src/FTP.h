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

int read_server_response(int socket_fd, char *buffer, size_t size);

int send_command(int socket_fd, const char *command, const char *argument);

int login_on_server(int socket_fd, const char *username, const char *password);
