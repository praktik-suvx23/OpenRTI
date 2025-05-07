#include "loggingFunctions.h"

std::string logMessage;
std::wstring logWmessage;

static std::string getLogFilePath(ShipTeam team) {
    #ifdef REDSHIP_LOG_PATH
        constexpr const char* redPath = REDSHIP_LOG_PATH;
    #else
        constexpr const char* redPath = "../log/defaultRedShipLog.txt";
    #endif
    
    #ifdef BLUESHIP_LOG_PATH
        constexpr const char* bluePath = BLUESHIP_LOG_PATH;
    #else
        constexpr const char* bluePath = "../log/defaultBlueShipLog.txt";
    #endif
    
    switch (team) {
        case ShipTeam::RED: return redPath;
        case ShipTeam::BLUE: return bluePath;
        default: return "../log/unknownTeamLog.txt";
    }
}

void initializeLogFile(const ShipTeam team) {
    std::string path = getLogFilePath(team);
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
    ofs << "[LOG INITIALIZED for " << (team == ShipTeam::RED ? "RED" : "BLUE") << "]\n";
}

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