#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>

std::pair<double, double> generateDoubleShipPosition(double lat, double lon, std::wstring team, int shipIndex, int totalShips);
std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter);
std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing);

#endif //HELPER_FUNCTIONS_H