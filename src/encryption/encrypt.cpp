#include "TEA.h"

std::string xorWithKey(const std::vector<uint8_t>& data, uint8_t key) {
    std::string result;
    for (uint8_t byte : data) {
        result += byte ^ key;
    }
    return result;
}
std::vector<uint8_t> fixedXOR(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    std::vector<uint8_t> result;
    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back(a[i] ^ b[i]);
    }
    return result;
}

std::string base64Encode(const std::vector<uint8_t>& data) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
        
    std::string result;
    size_t i = 0;
    while (i < data.size()) {
        uint32_t octet_a = i < data.size() ? data[i++] : 0;
        uint32_t octet_b = i < data.size() ? data[i++] : 0;
        uint32_t octet_c = i < data.size() ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        result += base64_chars[(triple >> 18) & 0x3F];
        result += base64_chars[(triple >> 12) & 0x3F];
        result += (i > data.size() + 1) ? '=' : base64_chars[(triple >> 6) & 0x3F];
        result += (i > data.size()) ? '=' : base64_chars[triple & 0x3F];
    }

    return result;
}

std::string bytesToHex(const std::vector<uint8_t>& bytes) {
    std::ostringstream oss;
    for (uint8_t byte : bytes) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }
    return oss.str();
}

int main() {

    std::string input1 = "1c0111001f010100061a024b53535009181c";
    std::string input2 = "686974207468652062756c6c277320657965";
    auto bytes1 = hexToBytes(input1);
    auto bytes2 = hexToBytes(input2);
    auto fixedXored = fixedXOR(bytes1, bytes2);

    std::string result = bytesToHex(fixedXored);

    std::cout << "Base64 result " << result << std::endl; 

    return 0;
}