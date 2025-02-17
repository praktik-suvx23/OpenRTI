#include "../include/carFederate.h"
#include "../include/carFedAmb.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <thread>

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

void startFederate(int index, const std::string& carFile) {
    std::wstring federateName = L"carFederate" + std::to_wstring(index);  // Unique name per federate

    try {
        carFederate myFederate;
        myFederate.runFederate(federateName, carFile);
    } catch (const std::exception& e) {
        std::wcerr << L"Exception in federate " << index << L": " << e.what() << std::endl;
    }
}

int main() {
    std::string configFolder = CAR_MODULE_PATH; 
    const int numFederates = 4;
    
    std::vector<std::string> carConfigs = getCarConfigFiles(configFolder);
    if (carConfigs.empty()) {
        std::cerr << "No .car configuration files found in " << configFolder << std::endl;
        return 1;
    }

    std::vector<std::thread> federateThreads;

    for (int i = 0; i < numFederates; ++i) {
        int carIndex = i % carConfigs.size();  // Cycle through car files if needed
        federateThreads.emplace_back(startFederate, i, carConfigs[carIndex]);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    for (auto& thread : federateThreads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}