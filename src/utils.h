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

#define MAX_SIZE 256

struct TCP_input {
    char user[MAX_SIZE];
    char password[MAX_SIZE];
    char host[MAX_SIZE];
    char path[MAX_SIZE];
    char filename[MAX_SIZE];
};

int get_input(struct TCP_input *input, char *argv);

int getIP(char *host, char *ip);