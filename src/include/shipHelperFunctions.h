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

std::pair<double, double> generateDoubleShipPosition(double lat, double lon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disLat(-0.060000, 0.060000); // Approx. 6500 meters in latitude
    std::uniform_real_distribution<> disLon(-0.060000, 0.060000); // Approx. 6500 meters in longitude

    double shipLat, shipLon;

    shipLat = lat + disLat(gen);
    shipLon = lon + disLon(gen);
  
    std::wstringstream wss;
    wss << shipLat << L"," << shipLon;
    return std::pair<double, double>(shipLat, shipLon);
}

std::wstring generateShipPosition(double publisherLat, double publisherLon);
std::wstring generateShootShipPosition(double lat, double lon);
std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter);
double toRadians(double degrees);
double toDegrees(double radians);
double getAngle(double currentDirection, double maxTurnRate);
std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing);

#endif //HELPER_FUNCTIONS_H