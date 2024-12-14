#include "utils.h"

int get_input(struct TCP_input *input, char *argv) {
    
    char *cursor = argv;

    if(strncmp(cursor, "ftp://", 6) != 0){
        printf("Error: URL must start with 'ftp://'\n");
        return -1;
    }

    cursor += 6;

    // Get User and password
    char *at = strchr(cursor, '@');

    if (at == NULL){
        strcpy(input->user, "anonymous");
        strcpy(input->password, "");
    } else {
        char *colon = strchr(cursor, ':');
        if(colon != NULL && colon < at){
            size_t user_len = colon - cursor;
            if(user_len > MAX_SIZE){
                printf("Error: User name too long\n");
                return -1;
            }
            strncpy(input->user, cursor, user_len);
            input->user[user_len] = '\0';

            size_t password_len = at - (colon + 1);
            if(password_len > MAX_SIZE){
                printf("Error: Password too long\n");
                return -1;
            }
            strncpy(input->password, colon + 1, password_len);
            input->password[password_len] = '\0';           
        } 
        cursor = at + 1;
    } 

    // Get host
    char *slash = strchr(cursor, '/');
    if(slash == NULL){
        printf("Error: URL must contain host\n");
        return -1;
    } else {
        size_t host_len = slash - cursor;
        if(host_len > MAX_SIZE){
            printf("Error: Host name too long\n");
            return -1;
        }
        strncpy(input->host, cursor, host_len);
        input->host[host_len] = '\0';
        cursor = slash + 1;
    }

    // Get path and filename
    char *last_slash = strrchr(cursor, '/');
    if (last_slash == NULL){
        strcpy(input->path, "");
        size_t file_len = strlen(cursor);
        if (file_len > MAX_SIZE){
            printf("-> File name is too long\n");
            return -1;
        }

        strcpy(input->filename, cursor);
    } else {
        size_t path_len = last_slash - cursor;
        if (path_len > MAX_SIZE){
            printf("Error: Path name too long\n");
            return -1;
        }
        strncpy(input->path, cursor, path_len);
        input->path[path_len] = '\0';

        size_t filename_len = strlen(last_slash + 1);
        if(filename_len > MAX_SIZE){
            printf("Error: Filename too long\n");
            return -1;
        }
        strcpy(input->filename, last_slash + 1);
        input->filename[filename_len] = '\0';
    } 

    return 0;
}

int getIP(char *host, char *ip){
    struct hostent *h;
    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    strcpy(ip, inet_ntoa(*((struct in_addr *) h->h_addr)));

    printf("Host name  : %s\n", h->h_name);
    printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) h->h_addr)));

    return 0;
}

