#include <string>
#include <utility>
#include <cstring>
#include <arpa/inet.h> // For htonl and htons

struct Ship {
    std::string Name;
    std::pair<double, double> Position;
    double speed;
    double size;

    // Serialize the Ship struct into a byte array
    void serialize(char* buffer) const {
        // Serialize Name (fixed size for simplicity, e.g., 50 bytes)
        std::memset(buffer, 0, 50);
        std::memcpy(buffer, Name.c_str(), std::min(Name.size(), size_t(49)));
        buffer += 50;

        // Serialize Position (two doubles)
        std::memcpy(buffer, &Position.first, sizeof(double));
        buffer += sizeof(double);
        std::memcpy(buffer, &Position.second, sizeof(double));
        buffer += sizeof(double);

        // Serialize speed
        std::memcpy(buffer, &speed, sizeof(double));
        buffer += sizeof(double);

        // Serialize size
        std::memcpy(buffer, &size, sizeof(double));
    }
};