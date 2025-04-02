#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include "MissileSharedLibrary.cpp"

#define PORT 12345

void send_missile(int client_socket, const Missile& missile) {
    // Serialize missile to buffer
    char buffer[232] = {0};
    serializeMissile(missile, buffer);

    // Prefix message with its length (4-byte header)
    uint32_t msg_size = 232;
    send(client_socket, &msg_size, sizeof(msg_size), 0);  // Send size first
    send(client_socket, buffer, msg_size, 0);  // Then send full missile data
}
