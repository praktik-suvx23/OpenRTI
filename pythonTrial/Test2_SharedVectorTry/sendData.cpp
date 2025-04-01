#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#define PORT 12345

// Function to send the multimap over the socket
void send_multimap(int client_socket, const std::multimap<std::pair<int, int>, std::vector<double>>& rangeToTarget) {
    // Send the size of the multimap
    int map_size = rangeToTarget.size();
    send(client_socket, &map_size, sizeof(map_size), 0);

    // Iterate through the multimap and send each entry
    for (const auto& [pair, ranges] : rangeToTarget) {
        // Send the pair (shooter index and target index)
        send(client_socket, &pair.first, sizeof(pair.first), 0);
        send(client_socket, &pair.second, sizeof(pair.second), 0);

        // Send the size of the vector (should always be 10 in this case)
        int vector_size = ranges.size();
        send(client_socket, &vector_size, sizeof(vector_size), 0);

        // Send the vector of doubles
        send(client_socket, ranges.data(), vector_size * sizeof(double), 0);
    }
}

int main() {
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    // Bind the socket to the port
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Socket bind failed." << std::endl;
        close(server_fd);
        return 1;
    }

    // Listen for a connection
    if (listen(server_fd, 1) < 0) {
        std::cerr << "Socket listen failed." << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Waiting for a connection on port " << PORT << "..." << std::endl;

    // Accept a connection
    int client_socket = accept(server_fd, nullptr, nullptr);
    if (client_socket < 0) {
        std::cerr << "Failed to accept connection." << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Client connected. Sending data..." << std::endl;

    // Create a multimap with sample data
    std::multimap<std::pair<int, int>, std::vector<double>> rangeToTarget = {
        {{1, 2}, {100.5, 110.5, 120.5, 130.5, 140.5, 150.5, 160.5, 170.5, 180.5, 190.5}},
        {{3, 4}, {200.3, 210.3, 220.3, 230.3, 240.3, 250.3, 260.3, 270.3, 280.3, 290.3}},
        {{5, 6}, {300.7, 310.7, 320.7, 330.7, 340.7, 350.7, 360.7, 370.7, 380.7, 390.7}}
    };

    // Send the multimap to the client
    send_multimap(client_socket, rangeToTarget);

    std::cout << "Data sent. Closing connection." << std::endl;

    // Close the connection
    close(client_socket);
    close(server_fd);

    return 0;
}