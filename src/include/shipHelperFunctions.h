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

#endif //HELPER_FUNCTIONS_H