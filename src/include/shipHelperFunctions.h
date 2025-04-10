#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>

std::pair<double, double> generateDoubleShipPosition(std::pair<double, double> pos, std::wstring team, int row, int col);
int getOptimalShipsPerRow(int amountOfShips);
double calculateBearing(const std::pair<double, double>& source, const std::pair<double, double>& target);
std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter);                      // Not in use?
std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing);    // Not in use?

#endif //HELPER_FUNCTIONS_H