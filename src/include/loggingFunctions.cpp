#include "loggingFunctions.h"

std::string logMessage;
std::wstring logWmessage;

// Function to get the log file path based on the team
static std::string getLogFilePath(ShipTeam team) {
    #ifdef REDSHIP_LOG_PATH
        constexpr const char* redPath = REDSHIP_LOG_PATH;
    #else
        constexpr const char* redPath = "src/log/defaultRedShipLog.txt";
    #endif
    
    #ifdef BLUESHIP_LOG_PATH
        constexpr const char* bluePath = BLUESHIP_LOG_PATH;
    #else
        constexpr const char* bluePath = "src/log/defaultBlueShipLog.txt";
    #endif
    
    switch (team) {
        case ShipTeam::RED: return redPath;
        case ShipTeam::BLUE: return bluePath;
        default: return "src/log/unknownTeamLog.txt";
    }
}

// Function to initialize the log file using the team as a parameter
void initializeLogFile(const ShipTeam team) {
    std::string path = getLogFilePath(team);
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
    ofs << "[LOG INITIALIZED for " << (team == ShipTeam::RED ? "RED" : "BLUE") << "]\n";
}

// Function to log messages to the file using the team as a parameter
void logToFile(const std::string& message, ShipTeam team) {
    std::string path = getLogFilePath(team);
    std::ofstream ofs(path, std::ios::app);
    if (!ofs) {
        std::wcout << L"[ERROR-logToFile] Failed to open log file: " << std::wstring(path.begin(), path.end()) << std::endl;
        return;
    }
    if (ofs.is_open()) {
        ofs << message << std::endl;
    }
}

// Function to log wide string messages to the file using the team as a parameter
void wstringToLog(const std::wstring& wstr, ShipTeam team) {
    std::string str(wstr.begin(), wstr.end());
    logToFile(str, team);
}

void initializeLogFile() {
    std::string path = ADMIN_LOG_PATH;
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
    ofs << "[LOG INITIALIZED]\n";
}

void logToFile(const std::string& message) {
    std::ofstream ofs(ADMIN_LOG_PATH, std::ios::app);
    if (ofs.is_open()) {
        ofs << message << std::endl;
    }
}

void wstringToLog(const std::wstring& wstr) {
    std::string str(wstr.begin(), wstr.end());
    logToFile(str);
}