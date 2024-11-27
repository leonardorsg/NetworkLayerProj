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


int connect_to_server(char* server_address);

int read_server_response(int sockfd);
