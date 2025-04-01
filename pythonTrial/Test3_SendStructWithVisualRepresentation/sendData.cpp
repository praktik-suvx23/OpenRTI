#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include "sharedLibrary.cpp"

#define PORT 12345

void send_ship(int client_socket, const Ship& ship) {
    // Serialize the Ship struct
    char buffer[1024]; // Allocate enough space for serialization
    ship.serialize(buffer);

    // Send the serialized data
    send(client_socket, buffer, sizeof(buffer), 0);
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

    // Create a vector of Ship structs with unique data
    std::vector<Ship> ships = {
        {"ShipA", {10.5, 20.5}, 30.0, 40.0},
        {"ShipB", {50.5, 60.5}, 70.0, 80.0},
        {"ShipC", {90.5, 100.5}, 110.0, 120.0}
    };

    // Send the number of ships (total instances to be sent)
    int num_ships = ships.size() * 3; // Each ship will be sent 3 times
    send(client_socket, &num_ships, sizeof(num_ships), 0);

    // Send each ship at 3 different positions with slightly updated speeds
    for (const auto& ship : ships) {
        for (int i = 0; i < 3; ++i) {
            Ship updated_ship = ship;

            // Update the position (add an offset to the original position)
            updated_ship.Position.first += i * 10.0;  // Offset X by 5.0 * i
            updated_ship.Position.second += i * 15.0; // Offset Y by 10.0 * i

            // Update the speed (increase by 5.0 * i)
            updated_ship.speed += i * 10.0;

            // Send the updated ship
            send_ship(client_socket, updated_ship);
        }
    }

    std::cout << "Data sent. Closing connection." << std::endl;

    // Close the connection
    close(client_socket);
    close(server_fd);

    return 0;
}