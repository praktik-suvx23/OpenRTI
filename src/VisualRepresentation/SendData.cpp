#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/select.h>
#include <fcntl.h>
#include "SerializeData.cpp"

#define MISSILE_PORT 12345
#define REDSHIP_PORT 12346
#define BLUESHIP_PORT 12347
#define HEARTBEAT_PORT 12348

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

bool listenForHeartbeat(int heartbeat_socket) {
    static int client_socket = -1;
    static bool connected = false;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if (!connected) {
        int flags = fcntl(heartbeat_socket, F_GETFL, 0);
        fcntl(heartbeat_socket, F_SETFL, flags | O_NONBLOCK);

        client_socket = accept(heartbeat_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return false;
            }
            std::cerr << "[ERROR - listenForHeartbeat] accept() failed\n";
            return false;
        }

        std::wcout << L"[DEBUG] Heartbeat connection established. Waiting for messages..." << std::endl;
        connected = true;
    }

    uint32_t msg_size = 0;
    int size_read = recv(client_socket, &msg_size, sizeof(msg_size), MSG_WAITALL);
    if (size_read != sizeof(msg_size)) {
        if (size_read == 0) {
            std::wcout << L"[INFO - listenForHeartbeat] Heartbeat client disconnected." << std::endl;
        } else {
            std::wcout << L"[ERROR - listenForHeartbeat] Failed to read message size\n";
        }
        close(client_socket);
        connected = false;
        client_socket = -1;
        return false;
    }

    int bytes_read = recv(client_socket, buffer, msg_size, MSG_WAITALL);
    if (bytes_read <= 0) {
        std::wcout << L"[ERROR - listenForHeartbeat] Failed to read message\n";
        close(client_socket);
        connected = false;
        client_socket = -1;
        return false;
    }

    std::string msg(buffer, bytes_read);
    if (msg == "0") {
        std::wcout << L"[HEARTBEAT RECEIVED] 'complete'. Closing connection." << std::endl;
        close(client_socket);
        connected = false;
        client_socket = -1;
        return false;
    } else if (msg == "1") {
        return true;
    } else {
        std::wcout << L"[ERROR - listenForHeartbeat] Unknown message: " << msg.c_str() << std::endl;
        return true;
    }
}