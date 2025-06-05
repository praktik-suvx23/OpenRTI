#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <algorithm>


struct TableData {
    double timeFlyingSimulationTime;
    double timeFlyingRealTime;
};

int main() {
    int numberOfMissiles = 0; // Initialize to 0
    int numberOfShips = 0; // Initialize to 0

    std::ifstream input(DATA_LOG_PATH);
    if (!input.is_open()) {
        std::cerr << "Failed to open file: " << DATA_LOG_PATH << std::endl;
        return 1;
    }

    std::string line;
    bool foundRedShips = false;
    bool foundBlueShips = false;

    std::vector<TableData> tableDataList;
    TableData currentData = {0.0, 0.0};

    while (std::getline(input, line)) {
        std::string lowerLine = line;
        std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);

        // Detect "--" to start a new TableData object
        if (lowerLine.find("--") != std::string::npos) {
            if (currentData.timeFlyingSimulationTime != 0.0 || currentData.timeFlyingRealTime != 0.0) {
                tableDataList.push_back(currentData);
                currentData = {0.0, 0.0};
            }
            continue;
        }

        // Only report "redships" and "blueships" once
        if (!foundRedShips && lowerLine.find("redships") != std::string::npos) {
            std::cout << "Found 'redships' in: " << line << std::endl;
            size_t pos = lowerLine.find("redships");
            pos = line.find_first_of("0123456789", pos + 8); // 8 = length of "redships"
            if (pos != std::string::npos) {
                int value = 0;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    numberOfShips += value;
                    std::cout << "Number of red ships: " << value << std::endl;
                }
            }
            foundRedShips = true;
        }
        if (!foundBlueShips && lowerLine.find("blueships") != std::string::npos) {
            std::cout << "Found 'blueships' in: " << line << std::endl;
            size_t pos = lowerLine.find("blueships");
            pos = line.find_first_of("0123456789", pos + 9); // 9 = length of "blueships"
            if (pos != std::string::npos) {
                int value = 0;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    numberOfShips += value;
                    std::cout << "Number of blue ships: " << value << std::endl;
                }
            }
            foundBlueShips = true;
        }

        // Search for "simulation time"
        if (lowerLine.find("simulation time") != std::string::npos) {
            size_t pos = lowerLine.find("simulation time");
            pos = line.find_first_of("0123456789.-", pos);
            if (pos != std::string::npos) {
                double value;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    std::cout << "Found 'simulation time': " << value << " in: " << line << std::endl;
                    currentData.timeFlyingSimulationTime = value;
                }
            }
        }
        // Search for "flight time (realtime): "
        if (lowerLine.find("flight time (realtime): ") != std::string::npos) {
            size_t pos = lowerLine.find("flight time (realtime): ");
            pos = line.find_first_of("0123456789.-", pos);
            if (pos != std::string::npos) {
                double value;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    std::cout << "Found 'flight time (realtime)': " << value << " in: " << line << std::endl;
                    currentData.timeFlyingRealTime = value;
                }
            }
        }
    }
    if (currentData.timeFlyingSimulationTime != 0.0 || currentData.timeFlyingRealTime != 0.0) {
        tableDataList.push_back(currentData);
    }
    input.close();

    // Create output directory if it doesn't exist
    std::string outputDir = "../src/TextToTable/Tables";
    std::string mkdirCmd = "mkdir -p " + outputDir;
    system(mkdirCmd.c_str());

    // Generate a unique filename using timestamp and number of ships
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << outputDir << "/table_data_"
        << "ships" << numberOfShips << "_"
        << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".csv";
    std::string outputFileName = oss.str();

    std::ofstream output(outputFileName);
    if (!output.is_open()) {
        std::cerr << "Failed to open output file: " << outputFileName << std::endl;
        return 1;
    }

    // Write a big header with the amount of ships
    output << "###############################################################\n";
    output << "#                   SHIP DATA SUMMARY                        #\n";
    output << "#   Total number of ships: " << numberOfShips << "\n";
    output << "###############################################################\n\n";

    // Write CSV header
    output << "timeFlyingSimulationTime,timeFlyingRealTime\n";
    for (const auto& data : tableDataList) {
        output << data.timeFlyingSimulationTime << "," << data.timeFlyingRealTime << "\n";
    }
    output.close();
    std::cout << "Table data written to " << outputFileName << std::endl;
}
