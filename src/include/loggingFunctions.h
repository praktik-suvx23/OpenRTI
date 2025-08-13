#ifndef LOGGINGFUNCTIONS_H
#define LOGGINGFUNCTIONS_H

#include <string>
#include <fstream>
#include <iostream>
#include <locale>
#include <codecvt>
#include <filesystem>

#include "../ShipFederate/Ship.h"

enum loggingType {
    LOGGING_DEFAULT,
    LOGGING_REDSHIP,
    LOGGING_BLUESHIP,
    LOGGING_ADMIN,
    LOGGING_MISSILE,
    LOGGING_PYLINK
};

void initializeLogFile(loggingType type);
void logToFile(const std::string& message, loggingType type);
void missileToLog(const std::wstring& wstr, const std::wstring& missileId);
void wstringToLog(const std::wstring& wstr, loggingType type);
void initializeMissileLogFolder();
std::wstring toWString(const std::string& str);

extern std::string logMessage;
extern std::wstring logWmessage;

#endif // LOGGINGFUNCTIONS_H