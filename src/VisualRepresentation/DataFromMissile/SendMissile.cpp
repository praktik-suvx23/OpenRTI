#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include "MissileSharedLibrary.cpp"

#define PORT 12345

void send_missile(int client_socket, const Missile& missile) {
    // Serialize the Missile struct
    char buffer[1024]; // Allocate enough space for serialization
    serializeMissile(missile, buffer);

    // Send the serialized data
    send(client_socket, buffer, sizeof(buffer), 0);
}