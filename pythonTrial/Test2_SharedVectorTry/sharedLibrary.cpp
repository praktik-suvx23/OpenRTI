#include <map>
#include <vector>
#include <utility>
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>

// Define the shared map
static std::map<std::string, std::pair<std::vector<int>, std::pair<int, int>>> shared_map;
static std::mutex shared_mutex; // Mutex for thread safety

extern "C" {

// Function to add an entry to the shared map
void add_entry(const char* key, const int* vector_data, int vector_size, int pair_first, int pair_second) {
    std::lock_guard<std::mutex> lock(shared_mutex);

    // Convert the input vector_data to a std::vector<int>
    std::vector<int> vec(vector_data, vector_data + vector_size);

    // Add the entry to the map
    shared_map[key] = {vec, {pair_first, pair_second}};
}

// Function to retrieve an entry from the shared map
int get_entry(const char* key, int* vector_data, int* vector_size, int* pair_first, int* pair_second) {
    std::lock_guard<std::mutex> lock(shared_mutex);

    auto it = shared_map.find(key);
    if (it == shared_map.end()) {
        return 0; // Key not found
    }

    // Extract the vector and pair
    const auto& vec = it->second.first;
    const auto& pair = it->second.second;

    // Copy the vector data to the output array
    *vector_size = vec.size();
    for (int i = 0; i < *vector_size; ++i) {
        vector_data[i] = vec[i];
    }

    // Copy the pair data
    *pair_first = pair.first;
    *pair_second = pair.second;

    return 1; // Success
}

// Function to print the entire shared map (for debugging)
void print_shared_map() {
    std::lock_guard<std::mutex> lock(shared_mutex);

    for (const auto& [key, value] : shared_map) {
        std::cout << "Key: " << key << "\nVector: ";
        for (int v : value.first) {
            std::cout << v << " ";
        }
        std::cout << "\nPair: (" << value.second.first << ", " << value.second.second << ")\n";
    }
}
}