#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <cstring>
#include <algorithm>

void blockFromBytes(const char* bytes, uint32_t* block) {
    memcpy(&block[0], bytes, 4);
    memcpy(&block[1], bytes + 4, 4);
}

void bytesFromBlock(const uint32_t* block, char* bytes) {
    memcpy(bytes, &block[0], 4);
    memcpy(bytes + 4, &block[1], 4);
}

void padInput(std::string& input) {
    size_t padding = 8 - (input.size() % 8);
    if (padding != 8) input.append(padding, '\0');
}

void keyFromChars(const char* k, uint32_t* key) {
    memcpy(key, k, 16);
}

std::vector<uint8_t> hexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    
    return bytes;
}