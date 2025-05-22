#include <iostream>
#include <limits>

enum class HeartbeatStatus {
    Complete = 0,
    Alive = 1,
    NoConnection = 2,
    AcceptFailed = 3,
    MessageSizeFailed = 4,
    MessageReadFailed = 5,
    UnknownMessage = 6,
    UnknownError = 7
};

bool userInteraction() {
    std::string input = "temp";
    while(true){
        std::getline(std::cin, input);
        if (input == "") {
            break;
        }
    }
    return true;
}

int getValidIntInput() {
    int input = 0;
    while (true) {
        std::cin >> input;
        if (std::cin.fail() || input <= 0) {
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::wcout << "Invalid input. Please enter a positive whole number: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            return input;
        }
    }
}

int getValidDoubleInput() {
    double input = 0.0;
    while (true) {
        std::cin >> input;
        if (std::cin.fail() || input <= 0) {
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::wcout << "Invalid input. Please enter a positive number: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            return input;
        }
    }
}

std::wstring to_wstring(HeartbeatStatus status) {
    switch (status) {
        case HeartbeatStatus::Complete: return L"Complete";
        case HeartbeatStatus::Alive: return L"Alive";
        case HeartbeatStatus::NoConnection: return L"No Connection";
        case HeartbeatStatus::AcceptFailed: return L"Accept Failed";
        case HeartbeatStatus::MessageSizeFailed: return L"Message Size Failed";
        case HeartbeatStatus::MessageReadFailed: return L"Message Read Failed";
        case HeartbeatStatus::UnknownMessage: return L"Unknown Message";
        case HeartbeatStatus::UnknownError: return L"Unknown Error";
        default: return L"Invalid Status";
    }
}

bool processHeartbeat(HeartbeatStatus status, bool& heartbeatReceived, int& lastCode,
                      std::chrono::high_resolution_clock::time_point& lastReceivedTime,
                      bool& connection) {
    if (status == HeartbeatStatus::NoConnection) return true;

    if (static_cast<int>(status) != lastCode) {
        lastCode = static_cast<int>(status);
        switch (status) {
            case HeartbeatStatus::Complete:
                std::wcout << L"[INFO] Heartbeat complete. Exiting admin loop." << std::endl;
                return false;
            case HeartbeatStatus::Alive:
                std::wcout << L"[INFO] Heartbeat alive." << std::endl;
                connection = true;
                break;
            case HeartbeatStatus::NoConnection:
                std::wcout << L"[INFO] No connection available. Trying again..." << std::endl;
                break;
            default:
                std::wcout << L"[ERROR] Unhandled heartbeat status: " << to_wstring(status) << std::endl;
                break;
        }
    }

    if (status == HeartbeatStatus::Alive) {
        lastReceivedTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeSinceLastData = currentTime - lastReceivedTime;

    if (timeSinceLastData.count() > 15.0 && !connection) {
        heartbeatReceived = false;
        std::wcout << L"[INFO] Heartbeat not setup. Running without heartbeat. Which means no data will be received and\n\t"
                   << L"'AdminFederate' will not be able to know when to exit. Proceeding with a 'while(true)' loop." << std::endl;
        return true;
    } else if (timeSinceLastData.count() > 15.0 && connection) {
        std::wcout << L"[INFO] No data available on any socket for a long time. Exiting..." << std::endl;
        return false;
    }
    return true;
}

HeartbeatStatus listenForHeartbeat(const int heartbeat_socket) {
    static int client_socket = -1;
    static bool connected = false;

    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if (!connected) {
        int flags = fcntl(heartbeat_socket, F_GETFL, 0);
        fcntl(heartbeat_socket, F_SETFL, flags | O_NONBLOCK);

        client_socket = accept(heartbeat_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                return HeartbeatStatus::NoConnection;
            }
            std::cerr << "[ERROR - listenForHeartbeat] accept() failed\n";
            return HeartbeatStatus::AcceptFailed;
        }

        std::wcout << L"[DEBUG] Heartbeat connection established. Waiting for messages..." << std::endl;
        connected = true;
    }

    uint32_t msg_size = 0;
    int size_read = recv(client_socket, &msg_size, sizeof(msg_size), MSG_WAITALL);
    if (size_read != sizeof(msg_size)) {
        if (size_read == 0) {
            std::wcout << L"[INFO - listenForHeartbeat] Heartbeat client disconnected." << std::endl;
        } else {
            std::wcout << L"[ERROR - listenForHeartbeat] Failed to read message size\n";
        }
        close(client_socket);
        connected = false;
        client_socket = -1;
        return HeartbeatStatus::MessageSizeFailed;
    }

    int bytes_read = recv(client_socket, buffer, msg_size, MSG_WAITALL);
    if (bytes_read <= 0) {
        std::wcout << L"[ERROR - listenForHeartbeat] Failed to read message\n";
        close(client_socket);
        connected = false;
        client_socket = -1;
        return HeartbeatStatus::MessageReadFailed;
    }

    std::string msg(buffer, bytes_read);
    if (msg == "0") {
        std::wcout << L"[HEARTBEAT RECEIVED] 'complete'. Closing connection." << std::endl;
        close(client_socket);
        connected = false;
        client_socket = -1;
        return HeartbeatStatus::Complete;
    } else if (msg == "1") {
        return HeartbeatStatus::Alive;
    } else {
        std::wcout << L"[ERROR - listenForHeartbeat] Unknown message: " << msg.c_str() << std::endl;
        return HeartbeatStatus::UnknownMessage;
    }

    std::wcout << L"[DEBUG - listenForHeartbeat] How odd... hmm" << std::endl;
    return HeartbeatStatus::UnknownError;
}
