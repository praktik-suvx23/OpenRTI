#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/select.h>
#include "SerializeData.cpp"

#define MISSILE_PORT 12345
#define REDSHIP_PORT 12346
#define BLUESHIP_PORT 12347

void send_missile(int client_socket, const Missile& missile) {
    //std::wcout << L"[DEBUG] Sending missile data to visual representation program." << std::endl;
    // Serialize missile to buffer
    char buffer[232] = {0};
    serializeMissile(missile, buffer);

    // Prefix message with its length (4-byte header)
    uint32_t msg_size = 232;
    send(client_socket, &msg_size, sizeof(msg_size), 0);  // Send size first
    //std::wcout << L"[DEBUG] Sending missile data of size: " << msg_size << std::endl;
    send(client_socket, buffer, msg_size, 0);  // Then send full missile data
    //std::wcout << L"[DEBUG] Missile data sent." << std::endl;
}

void send_ship(int client_socket, const Ship& ship) {
    //std::wcout << L"[DEBUG] Sending ship data to visual representation program." << std::endl;
    // Serialize ship to buffer
    char buffer[256] = {0}; // Adjust size to match the serialized data
    serializeShip(ship, buffer);

    // Prefix message with its length (4-byte header)
    uint32_t msg_size = 256; // Adjust size to match the buffer
    send(client_socket, &msg_size, sizeof(msg_size), 0);  // Send size first
    //std::wcout << L"[DEBUG] Sending ship data of size: " << msg_size << std::endl;
    send(client_socket, buffer, msg_size, 0);  // Then send full ship data
    //std::wcout << L"[DEBUG] Ship data sent." << std::endl;
}

// Function to check if data is available on a socket
bool check_data(int socket) {
    fd_set readfds;
    FD_ZERO(&readfds);          // Clear the set
    FD_SET(socket, &readfds);   // Add the socket to the set

    timeval timeout;
    timeout.tv_sec = 1;         // Set a 1-second timeout
    timeout.tv_usec = 0;

    // Use select() to check for data
    int activity = select(socket + 1, &readfds, nullptr, nullptr, &timeout);

    if (activity > 0 && FD_ISSET(socket, &readfds)) {
        // Data is available on the socket
        char buffer[1024]; // Temporary buffer to read the data
        recv(socket, buffer, sizeof(buffer), 0); // Read the data (not saved)
        return true;
    }

    std::wcout << L"[DEBUG] No data available on socket: " << socket << " activity: " << activity << std::endl;
    return false; // No data detected
}

bool isSocketTransmittingData(int socket) {
    fd_set readfds;
    FD_ZERO(&readfds);          // Clear the set
    FD_SET(socket, &readfds);   // Add the socket to the set

    timeval timeout;
    timeout.tv_sec = 1;         // Set a 1-second timeout
    timeout.tv_usec = 0;

    // Use select() to check for data
    int activity = select(socket + 1, &readfds, nullptr, nullptr, &timeout);

    if (activity > 0 && FD_ISSET(socket, &readfds)) {
        // Data is available to be read from the socket
        return true;
    }

    return false; // No data detected
}