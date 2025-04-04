#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
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