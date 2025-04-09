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

// Serialize missile to buffer with 232 bytes and 4-byte header
void send_missile(int client_socket, const Missile& missile) {
    char buffer[232] = {0};
    serializeMissile(missile, buffer);

    uint32_t msg_size = 232;
    send(client_socket, &msg_size, sizeof(msg_size), 0);  // Send size first
    send(client_socket, buffer, msg_size, 0);  // Then send full missile data
}

// Serialize missile to buffer with 256 bytes and 4-byte header
void send_ship(int client_socket, const Ship& ship) {
    char buffer[256] = {0};
    serializeShip(ship, buffer);

    uint32_t msg_size = 256;
    send(client_socket, &msg_size, sizeof(msg_size), 0);  // Send size first
    send(client_socket, buffer, msg_size, 0);  // Then send full ship data
}

// Used in AdminFederate.cpp
// Listen for heartbeat messages from the ships and missiles to check if data is still transmitting
int listenForHeartbeat(int heartbeat_socket) {
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
                return 2;  // Error code 2: No connection available
            }
            std::cerr << "[ERROR - listenForHeartbeat] accept() failed\n";
            return 3;  // Error code 3: Failed to connect with Heartbeat
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
        return 4;  // Error code 4: Failed to read message size
    }

    int bytes_read = recv(client_socket, buffer, msg_size, MSG_WAITALL);
    if (bytes_read <= 0) {
        std::wcout << L"[ERROR - listenForHeartbeat] Failed to read message\n";
        close(client_socket);
        connected = false;
        client_socket = -1;
        return 5;  // Error code 5: Failed to read message
    }

    std::string msg(buffer, bytes_read);
    if (msg == "0") {
        std::wcout << L"[HEARTBEAT RECEIVED] 'complete'. Closing connection." << std::endl;
        close(client_socket);
        connected = false;
        client_socket = -1;
        return 0;  // Return 0 for 'complete'
    } else if (msg == "1") {
        return 1;  // Return 1 for 'alive'
    } else {
        std::wcout << L"[ERROR - listenForHeartbeat] Unknown message: " << msg.c_str() << std::endl;
        return 6;  // Error code 6: Unknown message
    }
}