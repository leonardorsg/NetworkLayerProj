#include "FTP.h"

int connect_to_server(char* server_address){
    int control_socket;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(SERVER_PORT);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((control_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(-1);
    }

    printf("Created control socket.\n");

    /*connect to the server*/
    if (connect(control_socket,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }    

    printf("Connected to server.\n");

    return control_socket;
}


int read_server_response(int control_socket, char *buffer, size_t size) {
    printf("Reading server response...\n");
    ssize_t bytes_read = read(control_socket, buffer, size - 1);
    
    if (bytes_read < 0) {
        perror("read()");
        return -1;
    }

    buffer[bytes_read] = '\0'; 
    printf("Server response: %s\n", buffer);

    return atoi(buffer); 
}


int send_command(int control_socket, const char *command, const char *argument) {
    char buffer[MAX_RESPONSE_SIZE];

    if (argument != NULL) 
        snprintf(buffer, sizeof(buffer), "%s %s\r\n", command, argument);
    else 
        snprintf(buffer, sizeof(buffer), "%s\r\n", command);

    if (write(control_socket, buffer, strlen(buffer)) < 0) {
        perror("write()");
        return -1;
    }

    printf("Sent %s command. \n", command);

    return 0;
}

int interpret_response(int control_socket,int response){
    response = response / 100;
    while (1) {
        switch (response) {
            case 1:
                // expecting another reply
                return 1;
            case 2:
                // command was successful
                return 2;
            case 3:
                // expecting argument value
                return 3;
            case 4:
                // error in command, need to send command again
                printf("> Command wasn\'t accepted. Will try again... \n");
                break;
            case 5:
                printf("> Command wasn\'t accepted. Closing connection... \n");
                close(control_socket);
                exit(-1);
                break;
            default:
                printf("> Unexpected response code: %d. Closing connection.\n", response);
                close(control_socket);
                return -1;
        }
    }
}

int login_on_server(int control_socket, const char *username, const char *password) {
    char response[MAX_RESPONSE_SIZE];

    int tries = 0, max_tries = 3;

    while (tries < max_tries) {
        
        // Send USER command
        if (send_command(control_socket, "USER", username) < 0) 
            return -1;

        // Read server response
        if (read_server_response(control_socket, response, sizeof(response)) < 0)
            return -1;

        int code = atoi(response);

        int interpreted_response = interpret_response(control_socket, code);

        switch (interpreted_response) {
            case 2:
                printf("Login successful (no password needed).\n");
                return 0;
            case 3: // Send PASS command, 331 means password is required
                printf("Password required.\n");
                
                if (send_command(control_socket, "PASS", password) < 0) 
                    return -1;

                if (read_server_response(control_socket, response, sizeof(response)) < 0) 
                    return -1;

                code = atoi(response);
                interpreted_response = interpret_response(control_socket, code);

                switch (interpreted_response) {
                    case 2:
                        printf("PASS successful.\n");
                        return 0;
                    case 4:
                        tries++;
                        printf("Error during PASS, retrying USER...\n");
                        break;
                    default:
                        printf("Unexpected response to PASS: %s\n", response);
                        return -1;
                }
                break;
            case 4:
                tries++;
                printf("Error during USER, retrying USER...\n");
                break;
            default:
                printf("Unexpected response to USER: %s\n", response);
                return -1;
        }
    }

    printf("Failed to login after %d attempts.\n", max_tries);
    return -1;
}

int change_working_directory(int control_socket, const char *path) {
    if (send_command(control_socket, "CWD", path) != 0) {
        printf("Error sending CWD command.\n");
        close(control_socket);
        return -1;
    } 
            
    char response[MAX_RESPONSE_SIZE];
    if (read_server_response(control_socket, response, sizeof(response)) < 0) {
        printf("Failed to read server response for CWD.\n");
        close(control_socket);
        return -1;

    }
    
    int code = atoi(response);

    int interpreted_response = interpret_response(control_socket, code);
    
    if(interpreted_response != 2){
        printf("Unexpected response to CWD: %s\n", response);
        close(control_socket);
        return -1;        
    }

    printf("Working directory changed successfully.\n");
    return 0;
}

int send_pasv_command(int control_socket, char *pasv_ip, int *port) {
    char response[MAX_RESPONSE_SIZE];
    int tries = 0, max_tries = 3;

    while (tries < max_tries) {
        if (send_command(control_socket, "PASV", NULL) != 0) 
            return -1;

        if (read_server_response(control_socket, response, sizeof(response)) < 0) 
            return -1;

        int code = atoi(response);
        int interpreted_response = interpret_response(control_socket, code);

        switch (interpreted_response) {
            case 2:
                printf("PASV command successful.\n");

                // Parse IP and port from response
                char *start = strchr(response, '(');
                char *end = strchr(response, ')');
                if (start == NULL || end == NULL || start >= end) {
                    printf("Failed to parse PASV response.\n");
                    return -1;
                }

                char pasv_data[MAX_RESPONSE_SIZE];
                strncpy(pasv_data, start + 1, end - start - 1);
                pasv_data[end - start - 1] = '\0';

                int h1, h2, h3, h4, p1, p2;
                if (sscanf(pasv_data, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
                    printf("Failed to extract PASV data.\n");
                    return -1;
                }

                // Build IP address
                snprintf(pasv_ip, INET_ADDRSTRLEN, "%d.%d.%d.%d", h1, h2, h3, h4);

                // Calculate port
                *port = (p1 * 256) + p2;

                printf("Parsed PASV response: IP=%s, Port=%d\n", pasv_ip, *port);
                return 0;
            case 4:
                tries++;
                printf("Error on PASV, retrying (%d/%d)...\n", tries, max_tries);
                break;
            default:
                printf("Unexpected response to PASV: %s\n", response);
                return -1;
        } 
    }

    printf("Failed to complete PASV command after %d attempts.\n", max_tries);
    return -1;
}

int send_retr_command(int control_socket, const char *filename){
    
    char response[MAX_SIZE];
    int tries = 0, max_tries = 3;

    while (tries < max_tries) {
        if (send_command(control_socket, "RETR", filename) != 0)
            return -1;

        if (read_server_response(control_socket, response, sizeof(response)) < 0)
            return -1;

        int code = atoi(response);
        int interpreted_response = interpret_response(control_socket, code);

        if (interpreted_response == 2 || interpreted_response == 1){
            printf("RETR command successful.\n");
            return 0;
        } else if (interpreted_response == 4) {
            tries++;
            printf("Retrying RETR command...\n");
        } else {
            printf("Unexpected response to RETR command: %s\n", response);
            return -1;
        }  

    }

    printf("Failed to send RETR command after %d attempts.\n", max_tries);
    return -1;
}

int download_file(int data_socket, const char *filename){
    printf("Downloading file...\n");
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("fopen()");
        return -1;
    }

    char buffer[MAX_SIZE];
    int bytes_read;
    while ((bytes_read = read(data_socket,buffer, sizeof(buffer))) > 0) {
        if ((fwrite(buffer, 1, bytes_read, file)) != (size_t)bytes_read) {
            perror("fwrite()");
            fclose(file);
            return -1;
        }
        // printf("Wrote %d bytes.\n", bytes_read);
    }

    if (bytes_read < 0) {
        perror("read()");
        fclose(file);
        return -1;
    }

    printf("File downloaded successfully.\n");

    if(fclose(file) != 0){
        perror("fclose()");
        return -1;
    }

    return 0;
}

int close_socket(int socket){
    if (send_command(socket, "quit", NULL) != 0) {
        printf("Failed to send QUIT command.\n");
        return -1;
    }

    char response[MAX_SIZE];

    //Succesfull goodbye response is 221
    if (read_server_response(socket, response, sizeof(response)) != 221)
        return -1;
    return close(socket);

}

