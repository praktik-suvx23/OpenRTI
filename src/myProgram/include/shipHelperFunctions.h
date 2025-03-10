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
std::wstring generateShipPosition(double publisherLat, double publisherLon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disLat(-0.060000, 0.060000); // Approx. 6500 meters in latitude
    std::uniform_real_distribution<> disLon(-0.060000, 0.060000); // Approx. 6500 meters in longitude

    double shipLat, shipLon;

    shipLat = publisherLat + disLat(gen);
    shipLon = publisherLon + disLon(gen);
  
    std::wstringstream wss;
    wss << shipLat << L"," << shipLon;
    return wss.str();
}

std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter) {
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert degrees to radians
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

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

std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing) {
    std::vector<std::wstring> tokens = split(position, L',');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid position format");
    }

    double lat = std::stod(tokens[0]);
    double lon = std::stod(tokens[1]);

    const double R = 6371000; // Radius of the Earth in meters

    double distance = speed * 0.5; // Distance traveled in meters (since speed is in m/s and time is 1 second)
    bearing = 5.0;
    double bearingRad = toRadians(bearing); // Convert bearing to radians

    double latRad = toRadians(lat);
    double lonRad = toRadians(lon);

    double newLat = asin(sin(latRad) * cos(distance / R) + cos(latRad) * sin(distance / R) * cos(bearingRad));
    double newLon = lonRad + atan2(sin(bearingRad) * sin(distance / R) * cos(latRad), cos(distance / R) - sin(latRad) * sin(newLat));

    // Convert from radians to degrees
    newLat = toDegrees(newLat);
    newLon = toDegrees(newLon);

    std::wstringstream wss;
    wss << std::fixed << std::setprecision(8) << newLat << L"," << std::fixed << std::setprecision(8) << newLon;
    return wss.str();
}

#endif //HELPER_FUNCTIONS_H