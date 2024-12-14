
# FTP Client Project

## Overview

This project is an implementation of an FTP client written in C. It includes functionalities for connecting to an FTP server, authenticating with a username and password, changing the working directory, and downloading files using passive mode.

## Features

- Connect to an FTP server using a control socket.
- Authenticate using `USER` and `PASS` commands.
- Change the working directory (`CWD`).
- Enter passive mode (`PASV`) to establish a data connection.
- Download files (`RETR`) and save them locally.
- Handle server responses and interpret FTP response codes.
- Cleanly close the connection (`QUIT`).

## Files

### FTP.c
This file contains the main implementation of the FTP client. Below are the key functions:

#### `int connect_to_server(char* server_address)`
- Establishes a connection to the FTP server at the specified address.

#### `int read_server_response(int control_socket, char *buffer, size_t size)`
- Reads and interprets the server's response messages.

#### `int send_command(int control_socket, const char *command, const char *argument)`
- Sends FTP commands to the server.

#### `int interpret_response(int control_socket, int response)`
- Interprets server response codes and decides the next action.

#### `int login_on_server(int control_socket, const char *username, const char *password)`
- Authenticates the user by sending `USER` and `PASS` commands.

#### `int change_working_directory(int control_socket, const char *path)`
- Changes the server's current working directory to the specified path.

#### `int send_pasv_command(int control_socket, char *pasv_ip, int *port)`
- Sends the `PASV` command and parses the server's response to extract IP and port for the data connection.

#### `int send_retr_command(int control_socket, const char *filename)`
- Sends the `RETR` command to request a file download.

#### `int download_file(int data_socket, const char *filename)`
- Downloads the specified file using the established data connection and saves it locally.

#### `int close_socket(int socket)`
- Closes the control socket after sending the `QUIT` command.

### Directory Structure

- `src/`: Contains the source files.
- `build/`: Stores the compiled object files and executable.
- `downloads/`: Directory for storing downloaded files.

## Dependencies

- Standard C libraries such as `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<unistd.h>`, `<arpa/inet.h>`, and `<netinet/in.h>`.

## How to Build

1. Ensure you have a C compiler (e.g., `gcc`) installed.
2. Use the included Makefile to build the project:
   ```bash
   make
   ```
3. The executable will be located in the `build` directory as `download`. Make sure to be in build directory when running ./download command.

## How to Run

1. Run the program using the `make run` command:
   ```bash
   cd build

   ./download {SERVER_ADDRESS}
   ```
    ### Examples of Server Addresses (PUBLIC):
    - ftp://ftp.up.pt/pub/gnu/emacs/elisp-manual-21-2.8.tar.gz
    - ftp://demo:password@test.rebex.net/readme.txt
    - ftp://anonymous:anonymous@ftp.bit.nl/speedtest/100mb.bin

    ### Example of Server Addresess (FEUP's Lab):
    - ftp://rcom:rcom@ftp.netlab.fe.up.pt/README
    - ftp://rcom:rcom@ftp.netlab.fe.up.pt/pipe.txt
    - ftp://rcom:rcom@ftp.netlab.fe.up.pt/files/crab.mp4 

    **All the downloaded files will be stored in the ``/downloads`` directory


## Example Usage

### Connect to Server
```bash
Connected to server.
```

### Login
```bash
Username: testuser
Password: ******
Login successful.
```

### Change Directory
```bash
Changing working directory to /example/path
Working directory changed successfully.
```

### Download File
```bash
Downloading file example.txt...
File downloaded successfully.
```

## Error Handling
- The program includes error checks for socket operations, server responses, and file operations.
- If an error occurs, appropriate messages are displayed, and the program exits gracefully.

## Future Enhancements
- Support for secure FTP (FTPS).
- Implementation of active mode (PORT).
- Enhanced user interface for command interaction.
