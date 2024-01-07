#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main() {
    char server_message[256] = "You have reached the server";

    // create the server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8002);
    server_address.sin_addr.s_addr = INADDR_ANY;        // same as 0.0.0.0

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);       // n = number of users which can wait for connection

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);  // addr = address of client connection

    send(client_socket, server_message, sizeof(server_message), 0);

    close(server_socket);
    return 0;
}