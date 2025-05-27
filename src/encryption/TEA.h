#pragma once
#include "HelpEncrypt.h"

void tea_encrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = 0; 
    uint32_t delta = 0x9E3779B9; //Golden ratio

    for (int i = 0; i < 32; ++i) {
        sum += delta;
        v0 += (((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]));
        v1 += (((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]));
    }
    v[0] = v0;
    v[1] = v1;
}

void tea_decrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t delta = 0x9E3779B9;
    uint32_t sum = delta * 32;

    for (int i = 0; i < 32; i++) {
        v1 -= (((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]));
        v0 -= (((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]));
        sum -= delta;
    }

    v[0] = v0;
    v[1] = v1;
}

void TeaEncryptRun() {
        std::ofstream clearEncryptFile(ENCRYPTED_TXT_PATH, std::ios::trunc);
    std::ofstream clearDecryptFile(DECRYPTED_TXT_PATH, std::ios::trunc);

    std::string 
        message = "Cool encryption", 
        key;
    
    std::cout << "Enter 16-byte key (as 16 chars): ";

    while (true) {
        //std::getline(std::cin, key);
        //key = "a4k5mf8h7ndi239n"; // For testing, replace with std::getline(std::cin, key);
        key = "äppelpaj2345676"; // For testing, replace with std::getline(std::cin, key);
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
}

void breakEncryption() {

 std::ifstream file(ENCRYPTED_TXT_PATH, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open cipher.bin\n";
        return;
    }

    uint32_t v[2]; // 64-bit block
    file.read(reinterpret_cast<char*>(&v), 8);
    file.close();

    char testKey[17] = {0};

    // Brute-force 4-char ASCII key repeated 4x (95^4 = ~81 million keys)
    for (char a = 32; a < 127; ++a) {
        for (char b = 32; b < 127; ++b) {
            for (char c = 32; c < 127; ++c) {
                for (char d = 32; d < 127; ++d) {

                    // Fill key: [a,b,c,d] repeated to 16 bytes
                    for (int i = 0; i < 4; ++i) {
                        testKey[i * 4 + 0] = a;
                        testKey[i * 4 + 1] = b;
                        testKey[i * 4 + 2] = c;
                        testKey[i * 4 + 3] = d;
                    }

                    // Convert to uint32_t[4]
                    uint32_t key[4];
                    keyFromChars(testKey, key);

                    // Copy cipher block and try decryption
                    uint32_t testV[2] = {v[0], v[1]};
                    tea_decrypt(testV, key);

                    // Convert output to 8-char string
                    char plain[9];
                    std::memcpy(plain, testV, 8);
                    plain[8] = '\0';

                    // Check known plaintext
                    if (std::string(plain).rfind("Cool", 0) == 0) {
                        std::cout << "Key found: '" << std::string(testKey, 16) << "'\n";
                        std::cout << "Decrypted: " << plain << "\n";
                        return;
                    }
                    else {
                        static int count = 0;
                        if (++count % 1000000 == 0)
                        std::cout << "Tried key: '" << std::string(testKey, 16) << "' -> " << plain << "\n";
                    }
                }
            }
        }
    }

    std::cout << "Key not found.\n";
}