#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <cstring>
#include <algorithm>


std::string encryptXOR(const std::string& input, const std::string& key) {
    std::string output;
    for (size_t i = 0; i < input.size(); ++i) {
        output += input[i] ^ key[i % key.size()];
    }
    return output;
}

void tea_encrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0; 
    uint32_t delta = 0x9E3779B9; //Golden ratio

    for (int i = 0; i < 32; ++i) {
        sum += delta;
        v0 += ((v1 << 4) ^ (v1 >> 5)) + v1 ^ (sum + k[sum & 3]);
        v1 += ((v0 << 4) ^ (v0 >> 5)) + v0 ^ (sum + k[(sum>>11) & 3]);
    }
    v[0] = v0;
    v[1] = v1;
}

void tea_decrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t delta = 0x9E3779B9;
    uint32_t sum = delta * 32;
    for (int i = 0; i < 32; i++) {
        v1 -= ((v0 << 4) ^ (v0 >> 5)) + v0 ^ (sum + k[(sum>>11) & 3]);
        v0 -= ((v1 << 4) ^ (v1 >> 5)) + v1 ^ (sum + k[sum & 3]);
        sum -= delta;
    }
    v[0] = v0;
    v[1] = v1;
}

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

int main() {
    std::ofstream clearEncryptFile(ENCRYPTED_TXT_PATH, std::ios::trunc);
    std::ofstream clearDecryptFile(DECRYPTED_TXT_PATH, std::ios::trunc);

    std::string 
        message = "Cool encryption", 
        key;
    
    std::cout << "Enter 16-byte key (as 16 chars): ";

    while (true) {
        //std::getline(std::cin, key);
        key = "a4k5mf8h7ndi239n"; // For testing, replace with std::getline(std::cin, key);
        if (key.size() == 16) 
            break;
        else
            std::cout << "Key must be 16 characters long. Try again: ";
    }

    std::cout << "Key accepted!" << std::endl;


    uint32_t keyArray[4];
    memcpy(keyArray, key.data(), 16);
    padInput(message);
    //std::string encryptedMessage = encryptXOR(message, key);

    std::string encryptedMessage;
    encryptedMessage.resize(message.size());

    for (size_t i = 0; i < message.size(); i += 8) {
        uint32_t block[2];
        blockFromBytes(&message[i], block);
        tea_encrypt(block, keyArray);
        bytesFromBlock(block, &encryptedMessage[i]);
    }

    std::ofstream outFile(ENCRYPTED_TXT_PATH, std::ios::binary);
    if (outFile) {
        outFile.write(encryptedMessage.data(), encryptedMessage.size());
        outFile.close();
        std::cout << "Encrypted message written!" << std::endl;
    } 
    else {
        std::cerr << "Error opening file for writing." << std::endl;
    }

    std::ifstream inFile(ENCRYPTED_TXT_PATH, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return 1;
    }
    std::string fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::string decryptedMessage = fileData;
    for (size_t i = 0; i < decryptedMessage.size(); i += 8) {
        uint32_t block[2];
        blockFromBytes(&decryptedMessage[i], block);
        tea_decrypt(block, keyArray);
        bytesFromBlock(block, &decryptedMessage[i]);
    }

    while (!decryptedMessage.empty() && decryptedMessage.back() == '\0') {
        decryptedMessage.pop_back();
    }

    std::cout << "Decrypted message: " << decryptedMessage << std::endl;

    std::ofstream outFileDecrypt(DECRYPTED_TXT_PATH, std::ios::out);
    if (outFileDecrypt) {
        outFileDecrypt.write(decryptedMessage.c_str(), decryptedMessage.size());
        outFileDecrypt.close();
        std::cout << "Decrypted message written!" << std::endl;
    } 

    return 0;
}