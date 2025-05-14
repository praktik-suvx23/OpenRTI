#ifndef LOGGINGFUNCTIONS_H
#define LOGGINGFUNCTIONS_H

#include <string>
#include <fstream>
#include <iostream>

#include "../CombinedShip/Ship.h"

void initializeLogFile(ShipTeam team);
void initializeLogFile();
void logToFile(const std::string& message, ShipTeam team);
void wstringToLog(const std::wstring& wstr, ShipTeam team);
void logToFile(const std::string& message);
void wstringToLog(const std::wstring& wstr);

extern std::string logMessage;
extern std::wstring logWmessage;

#endif // LOGGINGFUNCTIONS_H