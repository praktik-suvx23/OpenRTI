#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <sys/stat.h>


struct TableData {
    double timeFlyingSimulationTime;
    double timeFlyingRealTime;
    double distanceToTarget; 
    double averageDistancePerSecond;
    double ActualTimeScale;
};

// Helper to check if file exists
bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

int main() {
    int numberOfMissiles = 0;
    int numberOfShips = 0;
    int simulationTimeScale = 0;

    std::ifstream input(DATA_LOG_PATH);
    if (!input.is_open()) {
        std::cerr << "Failed to open file: " << DATA_LOG_PATH << std::endl;
        return 1;
    }

    std::string line;
    bool foundRedShips = false;
    bool foundBlueShips = false;
    bool foundSimulationTimeScale = false;

    std::vector<TableData> tableDataList;
    TableData currentData = {0.0, 0.0, 0.0, 0.0, 0.0};

    while (std::getline(input, line)) {
        std::string lowerLine = line;
        std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);

        // Detect "--" to start a new TableData object
        if (lowerLine.find("--") != std::string::npos) {
            numberOfMissiles++;
            if (currentData.timeFlyingSimulationTime != 0.0 || currentData.timeFlyingRealTime != 0.0) {
                if (currentData.timeFlyingRealTime != 0.0) {
                    currentData.averageDistancePerSecond = currentData.distanceToTarget / currentData.timeFlyingRealTime;
                    currentData.ActualTimeScale = currentData.timeFlyingSimulationTime / currentData.timeFlyingRealTime;
                } else {
                    currentData.averageDistancePerSecond = 0.0;
                }
                tableDataList.push_back(currentData);
                currentData = {0.0, 0.0, 0.0, 0.0, 0.0}; // Reset all fields!
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
        if (!foundSimulationTimeScale && lowerLine.find("timescalefactor") != std::string::npos) {
            std::cout << "Found 'timescalefactor' in: " << line << std::endl;
            size_t pos = lowerLine.find("timescalefactor");
            pos = line.find_first_of("0123456789", pos + 15); // 15 = length of "timescalefactor"
            if (pos != std::string::npos) {
                int value = 0;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    simulationTimeScale = value;
                    std::cout << "timescalefactor: " << value << std::endl;
                }
            }
            foundSimulationTimeScale = true;
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
        if (lowerLine.find("original distance to target: ") != std::string::npos) {
            size_t pos = lowerLine.find("original distance to target: ");
            pos = line.find_first_of("0123456789.-", pos);
            if (pos != std::string::npos) {
                double value;
                std::istringstream iss(line.substr(pos));
                if (iss >> value) {
                    std::cout << "Found 'original distance to target': " << value << " in: " << line << std::endl;
                    currentData.distanceToTarget = value;
                }
            }
        }
    }
    if (currentData.timeFlyingSimulationTime != 0.0 || currentData.timeFlyingRealTime != 0.0) {
        if (currentData.timeFlyingRealTime != 0.0) {
        currentData.averageDistancePerSecond = currentData.distanceToTarget / currentData.timeFlyingRealTime;
        currentData.ActualTimeScale = currentData.timeFlyingSimulationTime / currentData.timeFlyingRealTime;
        } else {
            currentData.averageDistancePerSecond = 0.0;
        }
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
    output << "#   Simulation time scale factor: " << simulationTimeScale << "\n";
    output << "###############################################################\n\n";

    // Write CSV header
    output << "timeFlyingSimulationTime,timeFlyingRealTime,distanceToTarget,averageDistancePerSecond,ActualTimeScale\n";
    for (const auto& data : tableDataList) {
        output << data.timeFlyingSimulationTime << "," 
               << data.timeFlyingRealTime << ","
               << data.distanceToTarget << ","
               << data.averageDistancePerSecond << ","
               << data.ActualTimeScale << "\n";
    }
    output.close();
    std::cout << "Table data written to " << outputFileName << std::endl;

    // Calculate averages for JSON summary
    double avgTimeScale = 0.0;
    double avgAvgDistPerSec = 0.0;
    double avgSimTime = 0.0;
    double avgRealTime = 0.0;
    double avgDistToTarget = 0.0;
    if (!tableDataList.empty()) {
        double sumTimeScale = 0.0;
        double sumAvgDistPerSec = 0.0;
        double sumSimTime = 0.0;
        double sumRealTime = 0.0;
        double sumDistToTarget = 0.0;

        for (const auto& data : tableDataList) {
            sumTimeScale += data.ActualTimeScale;
            sumAvgDistPerSec += data.averageDistancePerSecond;
            sumSimTime += data.timeFlyingSimulationTime;
            sumRealTime += data.timeFlyingRealTime;
            sumDistToTarget += data.distanceToTarget;
        }

        avgTimeScale = sumTimeScale / tableDataList.size();
        avgAvgDistPerSec = sumAvgDistPerSec / tableDataList.size();
        avgSimTime = sumSimTime / tableDataList.size();
        avgRealTime = sumRealTime / tableDataList.size();
        avgDistToTarget = sumDistToTarget / tableDataList.size();

        std::cout << "Average ActualTimeScale: " << avgTimeScale << std::endl;
        std::cout << "Average averageDistancePerSecond: " << avgAvgDistPerSec << std::endl;
        std::cout << "Average timeFlyingSimulationTime: " << avgSimTime << std::endl;
        std::cout << "Average timeFlyingRealTime: " << avgRealTime << std::endl;
        std::cout << "Average distanceToTarget: " << avgDistToTarget << std::endl;
    }

    // Set JSON output directory
    std::string jsonOutputDir = "../src/TextToTable/TablesJSON";
    std::string mkdirJsonCmd = "mkdir -p " + jsonOutputDir;
    system(mkdirJsonCmd.c_str());

    // Use the same base filename, but in the new directory and with .json extension
    std::string baseFileName = outputFileName.substr(outputFileName.find_last_of('/') + 1);
    std::string jsonFileName = jsonOutputDir + "/" + baseFileName.substr(0, baseFileName.find_last_of('.')) + ".json";

    // Write detailed JSON output
    std::ostringstream jsonOss;
    jsonOss << "{\n";
    jsonOss << "  \"total_number_of_ships\": " << numberOfShips << ",\n";
    jsonOss << "  \"number_of_missiles\": " << numberOfMissiles << ",\n";
    jsonOss << "  \"simulation_time_scale_factor\": " << simulationTimeScale << ",\n";
    jsonOss << "  \"table_data\": [\n";
    for (size_t i = 0; i < tableDataList.size(); ++i) {
        jsonOss << "    {\n";
        jsonOss << "      \"timeFlyingSimulationTime\": " << tableDataList[i].timeFlyingSimulationTime << ",\n";
        jsonOss << "      \"timeFlyingRealTime\": " << tableDataList[i].timeFlyingRealTime << ",\n";
        jsonOss << "      \"distanceToTarget\": " << tableDataList[i].distanceToTarget << ",\n";
        jsonOss << "      \"averageDistancePerSecond\": " << tableDataList[i].averageDistancePerSecond << ",\n";
        jsonOss << "      \"ActualTimeScale\": " << tableDataList[i].ActualTimeScale << "\n";
        jsonOss << "    }";
        if (i + 1 < tableDataList.size()) jsonOss << ",";
        jsonOss << "\n";
    }
    jsonOss << "  ]\n";
    jsonOss << "}\n";

    std::ofstream jsonOut(jsonFileName);
    if (!jsonOut.is_open()) {
        std::cerr << "Failed to open JSON output file: " << jsonFileName << std::endl;
        return 1;
    }
    jsonOut << jsonOss.str();
    jsonOut.close();
    std::cout << "JSON data written to " << jsonFileName << std::endl;

    // Append averages to averages.json instead of overwriting
    std::string averagesFileName = jsonOutputDir + "/averages.json";
    std::vector<std::string> averagesEntries;

    // If file exists, read and parse existing entries
    if (fileExists(averagesFileName)) {
        std::ifstream in(averagesFileName);
        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();
        // Find the array start and end
        size_t start = content.find('[');
        size_t end = content.rfind(']');
        if (start != std::string::npos && end != std::string::npos && end > start) {
            std::string arrayContent = content.substr(start + 1, end - start - 1);
            std::istringstream ss(arrayContent);
            std::string entry;
            while (std::getline(ss, entry, '}')) {
                if (entry.find('{') != std::string::npos) {
                    entry = entry.substr(entry.find('{'));
                    // Remove trailing comma and whitespace
                    entry.erase(entry.find_last_not_of(", \n\r\t") + 1);
                    if (!entry.empty())
                        averagesEntries.push_back(entry + "}");
                }
            }
        }
    }

    // Prepare new averages entry
    std::ostringstream avgOss;
    avgOss << "    {\n";
    avgOss << "      \"numberOfShips\": " << numberOfShips << ",\n";
    avgOss << "      \"ActualTimeScale\": " << avgTimeScale << ",\n";
    avgOss << "      \"averageDistancePerSecond\": " << avgAvgDistPerSec << ",\n";
    avgOss << "      \"timeFlyingSimulationTime\": " << avgSimTime << ",\n";
    avgOss << "      \"timeFlyingRealTime\": " << avgRealTime << ",\n";
    avgOss << "      \"distanceToTarget\": " << avgDistToTarget << "\n";
    avgOss << "    }";
    averagesEntries.push_back(avgOss.str());

    // Write all entries back as a JSON array
    std::ofstream averagesOut(averagesFileName);
    if (!averagesOut.is_open()) {
        std::cerr << "Failed to open averages output file: " << averagesFileName << std::endl;
        return 1;
    }
    averagesOut << "{\n  \"averages\": [\n";
    for (size_t i = 0; i < averagesEntries.size(); ++i) {
        averagesOut << averagesEntries[i];
        if (i + 1 < averagesEntries.size()) averagesOut << ",";
        averagesOut << "\n";
    }
    averagesOut << "  ]\n}\n";
    averagesOut.close();
    std::cout << "Averages data written/appended to " << averagesFileName << std::endl;

    return 0;
}
