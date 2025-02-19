#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>

class MyShip {
public:
    MyShip(const std::string& config_file, const std::string& ship_id)
        : config_file(config_file), ship_id(ship_id), length(0), width(0), height(0), number_of_robots(0) {
        load_config();
    }

    void load_config() {
        std::ifstream file(config_file);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            parse_json(content);
        } else {
            std::cerr << "Failed to open file: " << config_file << std::endl;
        }
    }

    void display_info() const {
        std::cout << "Ship ID: " << ship_id << std::endl;
        std::cout << "Ship Size: Length=" << length << ", Width=" << width << ", Height=" << height << std::endl;
        std::cout << "Number of Robots: " << number_of_robots << std::endl;
    }

private:
    std::string config_file;
    std::string ship_id;
    int length;
    int width;
    int height;
    int number_of_robots;

    void parse_json(const std::string& content) {
        size_t pos = content.find("\"" + ship_id + "\":");
        if (pos != std::string::npos) {
            size_t start = content.find("{", pos);
            size_t end = content.find("}", start);
            std::string ship_data = content.substr(start, end - start + 1);

            std::unordered_map<std::string, int> values;
            std::istringstream ss(ship_data);
            std::string line;
            while (std::getline(ss, line)) {
                size_t colon = line.find(":");
                if (colon != std::string::npos) {
                    std::string key = line.substr(0, colon);
                    key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
                    key.erase(remove_if(key.begin(), key.end(), [](char c) { return c == '\"' || c == ',' || c == '{' || c == '}'; }), key.end());

                    std::string value = line.substr(colon + 1);
                    value.erase(remove_if(value.begin(), value.end(), isspace), value.end());
                    value.erase(remove_if(value.begin(), value.end(), [](char c) { return c == '\"' || c == ',' || c == '{' || c == '}'; }), value.end());

                    if (!key.empty() && !value.empty()) {
                        values[key] = std::stoi(value);
                    }
                }
            }

            length = values["Length"];
            width = values["Width"];
            height = values["Height"];

            // Find the NumberOfRobots value
            size_t robots_pos = ship_data.find("\"Robots\":");
            if (robots_pos != std::string::npos) {
                size_t robots_start = ship_data.find("{", robots_pos);
                size_t robots_end = ship_data.find("}", robots_start);
                std::string robots_data = ship_data.substr(robots_start, robots_end - robots_start + 1);

                std::istringstream robots_ss(robots_data);
                while (std::getline(robots_ss, line)) {
                    size_t colon = line.find(":");
                    if (colon != std::string::npos) {
                        std::string key = line.substr(0, colon);
                        key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
                        key.erase(remove_if(key.begin(), key.end(), [](char c) { return c == '\"' || c == ',' || c == '{' || c == '}'; }), key.end());

                        std::string value = line.substr(colon + 1);
                        value.erase(remove_if(value.begin(), value.end(), isspace), value.end());
                        value.erase(remove_if(value.begin(), value.end(), [](char c) { return c == '\"' || c == ',' || c == '{' || c == '}'; }), value.end());

                        if (key == "NumberOfRobots" && !value.empty()) {
                            number_of_robots = std::stoi(value);
                        }
                    }
                }
            }
        } else {
            std::cerr << "Ship ID not found: " << ship_id << std::endl;
        }
    }
};

void printShipInfo(const std::string& config_file, const std::string& ship_id) {
    MyShip ship(config_file, ship_id);
    ship.display_info();
}

