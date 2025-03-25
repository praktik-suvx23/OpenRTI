#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>

// Update this so it uses PositionRec and HLAfloat64BE
// see carProgram/carFederate/src/carFederate.cpp for example?

std::pair<double, double> generateDoubleShootShipPosition(double lat, double lon);
std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter);
std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing);

#endif //HELPER_FUNCTIONS_H