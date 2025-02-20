#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class JsonParser {
public:
    JsonParser(const std::string &filename) {
        inputFile.open(filename);
        if (!inputFile.is_open()) {
            std::cerr << "Failed to open " << filename << std::endl;
        }
    }

    bool isFileOpen() const {
        return inputFile.is_open();
    }

    void parseShipConfig(const std::string &shipId) {
        resetFile();
        std::string line;
        std::string currentShip;

        while (std::getline(inputFile, line)) {
            trim(line);
            if (line.find("\"" + shipId + "\"") != std::string::npos && line.find(":") != std::string::npos) {
                currentShip = extractKey(line);
                std::cout << currentShip << ":" << std::endl;
                parseShipDetails();
                break;
            }
        }
    }

private:
    std::ifstream inputFile;

    void resetFile() {
        inputFile.clear();
        inputFile.seekg(0);
    }

    void parseShipDetails() {
        std::string line;
        int bracketCount = 0;

        while (std::getline(inputFile, line)) {
            trim(line);
            bracketCount += std::count(line.begin(), line.end(), '{');
            bracketCount -= std::count(line.begin(), line.end(), '}');

            if (line.find("Length") != std::string::npos) {
                std::cout << "  Length: " << extractValue(line) << " meters" << std::endl;
            } else if (line.find("Width") != std::string::npos) {
                std::cout << "  Width:  " << extractValue(line) << " meters" << std::endl;
            } else if (line.find("Height") != std::string::npos) {
                std::cout << "  Height: " << extractValue(line) << " meters" << std::endl;
            } else if (line.find("NumberOfRobots") != std::string::npos) {
                std::cout << "  Number of Robots: " << extractValue(line) << std::endl;
            }

            if (bracketCount == 0) break;
        }
    }

    std::string extractKey(const std::string &line) {
        std::size_t keyStart = line.find("\"") + 1;
        std::size_t keyEnd = line.find("\":", keyStart);
        return (keyEnd != std::string::npos) ? line.substr(keyStart, keyEnd - keyStart) : "";
    }

    std::string extractValue(const std::string &line) {
        std::size_t colonPos = line.find(":");
        if (colonPos != std::string::npos) {
            std::string value = line.substr(colonPos + 1);
            trim(value);
            return value;
        }
        return "";
    }

    void trim(std::string &str) {
        str.erase(0, str.find_first_not_of(" \t\n\r"));
        str.erase(str.find_last_not_of(" \t\n\r") + 1);
    }
};

#endif // JSONPARSER_H

/*
Example usage in main.cpp:

#include "JsonParser.h"

int main() {
    JsonParser parser("/usr/OjOpenRTI/OpenRTI/src/myProgram/ShipData/ShipData.json");
    if (!parser.isFileOpen()) return 1;

    parser.parseShipConfig("Ship1");
    parser.parseShipConfig("Ship2");
    return 0;
}

Compilation:
    g++ -std=c++17 -o json_parser main.cpp
*/