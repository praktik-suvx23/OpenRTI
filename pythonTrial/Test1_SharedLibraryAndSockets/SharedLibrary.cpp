#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

static int server_socket = -1; // Server socket

extern "C" {

// Function to initialize the socket server
void initialize_socket_server(int port) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(server_socket);
        server_socket = -1;
        return;
    }

    if (listen(server_socket, 1) < 0) {
        std::cerr << "Failed to listen on socket." << std::endl;
        close(server_socket);
        server_socket = -1;
        return;
    }

    std::cout << "Socket server initialized on port " << port << std::endl;
}

// Function to send a value to the client
void send_value(int value) {
    if (server_socket == -1) {
        std::cerr << "Socket server not initialized." << std::endl;
        return;
    }

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0) {
        std::cerr << "Failed to accept client connection." << std::endl;
        return;
    }

    send(client_socket, &value, sizeof(value), 0);
    close(client_socket);
}

// Function to clean up the socket server
void cleanup_socket_server() {
    if (server_socket != -1) {
        close(server_socket);
        server_socket = -1;
    }
}
}