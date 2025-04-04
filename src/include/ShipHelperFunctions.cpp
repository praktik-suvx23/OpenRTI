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


std::pair<double, double> generateDoubleShipPosition(double lat, double lon, std::wstring team, int index) {

    const double latitudeOffset = 2500.0 / 111000.0;
    const double longitudeOffset = 2500.0 / 111000.0 * cos(lat * M_PI / 180);


    double shipLat, shipLon;

    if (team == L"Blue") {
        //Blue team
        shipLat = lat + latitudeOffset + 0.01 * index;
        shipLon = lon + longitudeOffset;
    } else {
        //Red team
        shipLat = lat - latitudeOffset + 0.01 * index;
        shipLon = lon - longitudeOffset;
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


