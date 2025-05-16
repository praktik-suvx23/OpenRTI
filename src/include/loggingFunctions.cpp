#include "loggingFunctions.h"

std::string logMessage;
std::wstring logWmessage;

// Function to get the log file path based on the type
static std::string getLogFilePath(loggingType type) {
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

    #ifdef ADMIN_LOG_PATH
        constexpr const char* adminPath = ADMIN_LOG_PATH;
    #else
        constexpr const char* adminPath = "src/log/defaultAdminLog.txt";
    #endif

    #ifdef PYLINK_LOG_PATH
        constexpr const char* pyLinkPath = PYLINK_LOG_PATH;
    #else
        constexpr const char* pyLinkPath = "src/log/defaultPyLinkLog.txt";
    #endif

    #ifdef MISSILE_LOG_PATH
        constexpr const char* missilePath = MISSILE_LOG_PATH;
    #else
        constexpr const char* missilePath = "src/log/defaultMissileLog.txt";
    #endif
    
    switch (type) {
        case loggingType::LOGGING_DEFAULT: return "src/log/unknownTeamLog.txt";
        case loggingType::LOGGING_REDSHIP: return redPath;
        case loggingType::LOGGING_BLUESHIP: return bluePath;
        case loggingType::LOGGING_MISSILE: return missilePath;
        case loggingType::LOGGING_PYLINK: return pyLinkPath;
        case loggingType::LOGGING_ADMIN: return adminPath;
        default: return "src/log/unknownTeamLog.txt";
    }
}

// Function to initialize the log file using the team as a parameter
void initializeLogFile(const loggingType type) {
    std::string path = getLogFilePath(type);
    std::ofstream ofs(path, std::ofstream::out | std::ofstream::trunc);
}

// Function to log messages to the file using the team as a parameter
void logToFile(const std::string& message, loggingType type) {
    std::string path = getLogFilePath(type);
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
void wstringToLog(const std::wstring& wstr, loggingType type) {
    std::string str(wstr.begin(), wstr.end());
    logToFile(str, type);
}