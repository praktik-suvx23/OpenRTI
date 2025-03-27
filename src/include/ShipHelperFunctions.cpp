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


std::pair<double, double> generateDoubleShipPosition(double lat, double lon, std::wstring team) {

    const double latitudeOffset = 6000.0 / 111000.0;
    const double longitudeOffset = 6000.0 / 111000.0 * cos(lat * M_PI / 180);

    double shipLat, shipLon;

    if (team == L"Blue") {
        //Blue team
        shipLat = lat + latitudeOffset;
        shipLon = lon + longitudeOffset;
    } else {
        //Red team
        shipLat = lat - latitudeOffset;
        shipLon = lon - longitudeOffset;
    }
  
    return std::pair<double, double>(shipLat, shipLon);
}

std::pair<double, double> generateDoubleShootShipPosition(double lat, double lon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disLat(-0.060000, 0.060000); // Approx. 6500 meters in latitude
    std::uniform_real_distribution<> disLon(-0.060000, 0.060000); // Approx. 6500 meters in longitude

    double shipLat, shipLon;

    shipLat = lat + disLat(gen);
    shipLon = lon + disLon(gen);

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


