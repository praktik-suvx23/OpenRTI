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