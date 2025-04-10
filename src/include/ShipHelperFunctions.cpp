#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <iomanip>

#include "MissileCalculator.h"
#include "shipHelperFunctions.h"

// Update this so it uses PositionRec and HLAfloat64BE
// see carProgram/carFederate/src/carFederate.cpp for example?


std::pair<double, double> generateDoubleShipPosition(std::pair<double, double> basePos, std::wstring team, int row, int col) {
    const double boxSizeMeters = 500.0;  // Each box is 500m x 500m
    const double borderSizeMeters = 25.0;  // 25m border around each box
    const double boxSizeDegrees = boxSizeMeters / 111000.0;  // Convert meters to degrees
    const double borderSizeDegrees = borderSizeMeters / 111000.0;

    // Random number generator for positions within the box
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> randomOffset(borderSizeDegrees, boxSizeDegrees - borderSizeDegrees);

    // Calculate the base position for the current box
    double boxLat = basePos.first + row * boxSizeDegrees;
    double boxLon = basePos.second + col * boxSizeDegrees;

    // Add random offsets within the box
    double shipLat = boxLat + randomOffset(gen);
    double shipLon = boxLon + randomOffset(gen);

    // Adjust for team placement
    if (team == L"Red") {
        shipLon -= 0.072;  // Move Red team 8000m west (approx. 0.072 degrees)
    }

    return std::make_pair(shipLat, shipLon);
}

int getOptimalShipsPerRow(int amountOfShips) {
    if (amountOfShips % 5 == 0)
        return 5;
    else if (amountOfShips % 4 == 0)
        return 4;
    else if (amountOfShips % 3 == 0)
        return 3;
    else
        return std::min(7, amountOfShips);
}

double calculateBearing(const std::pair<double, double>& source, const std::pair<double, double>& target) {
    double lat1 = toRadians(source.first); // Convert to radians
    double lon1 = toRadians(source.second);
    double lat2 = toRadians(target.first);
    double lon2 = toRadians(target.second);

    // Calculate the bearing
    double dLon = lon2 - lon1;
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double bearing = atan2(y, x);  // atan2 is used to get the correct quadrant
    bearing = toDegrees(bearing);  // Convert back to degrees
    if (bearing < 0) bearing += 360.0;  // Normalize bearing to [0, 360]

    return bearing;
}
