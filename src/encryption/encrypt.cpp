#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>


std::string encrypt(const std::string& input, const std::string& key) {
    std::string output;
    for (size_t i = 0; i < input.size(); ++i) {
        output += input[i] ^ key[i % key.size()];
    }
    return output;
}

int main() {
    std::ofstream clearEncryptFile(ENCRYPTED_TXT_PATH, std::ios::trunc);
    std::ofstream clearDecryptFile(DECRYPTED_TXT_PATH, std::ios::trunc);

    std::string 
        message = "Cool encryption", 
        key;
    
    std::cout << "Enter the key: ";

    std::getline(std::cin, key);

    std::string encryptedMessage = encrypt(message, key);

    std::ofstream outFile(ENCRYPTED_TXT_PATH, std::ios::app);
    
    if (outFile) {
        outFile.write(encryptedMessage.c_str(), encryptedMessage.size());
        outFile.close();
        std::cout << "Encrypted message written!" << std::endl;
    } 
    else {
        std::cerr << "Error opening file for writing." << std::endl;
    }

    std::ifstream inFile(ENCRYPTED_TXT_PATH, std::ios::app);
    if (inFile) {
        std::string fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        fileData = encrypt(fileData, key);
        std::ofstream outFile(DECRYPTED_TXT_PATH, std::ios::app);
        if (outFile) {
            outFile.write(fileData.c_str(), fileData.size());
            outFile.close();
            std::cout << "Decrypted message written!" << std::endl;
        } 
    }

    return 0;
}