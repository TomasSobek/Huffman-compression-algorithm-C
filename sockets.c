#include "sockets.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <malloc.h>

static int create_TCP_IPV4_socket(void) {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int create_server_socket(unsigned int port) {
    int server_socket;
    struct sockaddr_in server_address;

    // creating server socket
    if ((server_socket = create_TCP_IPV4_socket()) == -1) {
        perror("Error creating server socket.");
        exit(EXIT_FAILURE);
    }

    // setting up server ip address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // try bind socket
    if (bind(server_socket,(struct sockaddr *) &server_address,sizeof(server_address)) == -1) {
        perror("Error binding server socket.");
        exit(EXIT_FAILURE);
    }

    // listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening on server socket.");
        exit(EXIT_FAILURE);
    }
    printf("Server socket created and listing on port %d...\n", port);

    return server_socket;
}

int create_client_socket(const char *server_ip, unsigned int port) {
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = create_TCP_IPV4_socket()) == -1) {
        perror("Error creating client socket.");
        exit(EXIT_FAILURE);
    }

    // setting up server ip address structure
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (strlen(server_ip) == 0) {
        server_address.sin_addr.s_addr = INADDR_ANY;
    } else {
        server_address.sin_addr.s_addr = inet_addr(server_ip);
    }

    // connect to server
    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server.");
        exit(EXIT_FAILURE);
    }

    if (strlen(server_ip) == 0) {
        printf("Connected to server at localhost:%d.\n", port);
    } else {
        printf("Connected to server at %s:%d.\n", server_ip, port);
    }

    return client_socket;
}