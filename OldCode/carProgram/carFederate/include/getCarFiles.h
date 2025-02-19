#ifndef GET_CAR_FILES_H
#define GET_CAR_FILES_H

#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

std::vector<std::string> getCarConfigFiles(const std::string& folderPath) {
    std::vector<std::string> carFiles;

    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".car") {
                carFiles.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accessing folder: " << e.what() << std::endl;
    }

    return carFiles;
}

#endif