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


std::pair<double, double> generateDoubleShipPosition(double lat, double lon, std::wstring team, int shipIndex, int totalShips) {
    const double lineLengthMeters = 10000.0; // Line length in meters (10 km)
    const double metersPerDegreeLat = 111000.0; // Approx. meters per degree latitude
    const double metersPerDegreeLon = 111000.0 * cos(lat * M_PI / 180); // Adjust for longitude scaling based on latitude

    // Convert line length to degrees
    const double lineLengthDegreesLat = lineLengthMeters / metersPerDegreeLat;
    const double lineLengthDegreesLon = lineLengthMeters / metersPerDegreeLon;

    // Calculate the position along the line based on the ship index
    double fraction = static_cast<double>(shipIndex) / (totalShips - 1); // Fraction along the line (0 to 1)

    double shipLat, shipLon;

    if (team == L"Blue") {
        // Blue team: Horizontal line (vary longitude)
        shipLat = lat;
        shipLon = lon + fraction * lineLengthDegreesLon; // Spread ships along the line
    } else {
        // Red team: Vertical line (vary latitude)
        shipLat = lat + fraction * lineLengthDegreesLat; // Spread ships along the line
        shipLon = lon;
    }

    return std::pair<double, double>(shipLat, shipLon);
}

// Function to convert degrees to radians

double getAngle(double currentDirection, double maxTurnRate) {

    currentDirection = toRadians(currentDirection);
    // Calculate desired change in direction
    double desiredChange = 10.0 * sin(currentDirection * M_PI / 180); // Increase angle
    
    // Limit the change in direction to the maximum turn rate
    if (desiredChange > maxTurnRate) {
        desiredChange = maxTurnRate;
    } else if (desiredChange < -maxTurnRate) {
        desiredChange = -maxTurnRate;
    }
    
    currentDirection += toRadians(desiredChange);
    currentDirection = fmod(toDegrees(currentDirection) + 360, 360);
    return currentDirection;
}


