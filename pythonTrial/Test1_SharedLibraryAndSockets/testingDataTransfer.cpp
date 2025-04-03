#include <iostream>
#include <thread>
#include <chrono>
#include <dlfcn.h>

int main() {
    // Load the shared library
    void* handle = dlopen("./libSharedLibrary.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load shared library: " << dlerror() << std::endl;
        return 1;
    }

    // Get function pointers
    auto initialize_socket_server = (void (*)(int))dlsym(handle, "initialize_socket_server");
    auto send_value = (void (*)(int))dlsym(handle, "send_value");
    auto cleanup_socket_server = (void (*)())dlsym(handle, "cleanup_socket_server");

    if (!initialize_socket_server || !send_value || !cleanup_socket_server) {
        std::cerr << "Failed to load functions: " << dlerror() << std::endl;
        dlclose(handle);
        return 1;
    }

    // Initialize the socket server on port 12345
    initialize_socket_server(12345);

    struct CountingUp {
        int value;
    };

    CountingUp countingUp{0};

    for (int i = 0; i < 100; ++i) {
        countingUp.value = i + 1;
        send_value(countingUp.value); // Send the value to the client
        std::cout << "Sent value: " << countingUp.value << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Goodbye!" << std::endl;

    // Clean up the socket server
    cleanup_socket_server();

    // Close the shared library
    dlclose(handle);
    return 0;
}