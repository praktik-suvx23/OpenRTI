#ifndef MISSILECALCULATOR_H
#define MISSILECALCULATOR_H
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <iostream>

// External random number generator
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> speedDis;

// Function to get position
std::wstring getPosition(double &currentLatitude, double &currentLongitude) {
    currentLatitude = 20.4382900;
    currentLongitude = 15.6253400;
    return std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
}

// Function to get altitude
double getAltitude() {
    static double altitude = 50.0;
    static bool increasing = true;
    static double angle = 60.0;

    if (increasing) {
        altitude += 50.0 * sin(angle * M_PI / 180);
        if (altitude > 1000.0) {
            altitude = 1000.0;
            increasing = false;
        }
    } else {
        altitude -= 50.0 * sin(angle * M_PI / 180);
        if (altitude < 0.0) {
            altitude = 0.0;
            increasing = true;
        }
    }
    return altitude;
}

// Function to get fuel level
double getFuelLevel(double speed) {
    static double fuelLevel = 100.0;
    fuelLevel -= 0.01 * (speed / 100);
    if (fuelLevel < 0) {
        fuelLevel = 0;
    }
    return fuelLevel;
}

// Function to get speed
double getSpeed(double cSpeed, double minSpeed, double maxSpeed) {
    speedDis.param(std::uniform_real_distribution<>::param_type(cSpeed - 10.0, cSpeed + 10.0));
    double newSpeed = speedDis(gen);
    if (newSpeed < minSpeed) {
        newSpeed = minSpeed;
    } else if (newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }
    return newSpeed;
}

// Function to split a string
std::vector<std::wstring> split(const std::wstring &s, wchar_t delimiter) {
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Utility functions for conversions
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

// Function to reduce altitude
double reduceAltitude(double altitude, double speed, double distance) {
    double newAltitude = altitude;
    if (distance == 0) {
        return altitude;
    }
    double descentRate = 45.0;
    double descentDistance = distance - speed * 0.5;
    newAltitude = altitude - descentDistance * tan(toRadians(descentRate));
    if (newAltitude < 0) {
        newAltitude = 0;
        std::wcout << L"Altitude below 0 not allowed" << std::endl;
    }
    return newAltitude;
}

// Function to calculate new position
std::pair<double, double> calculateNewPosition(const std::pair<double, double> &position, double speed, double bearing) {
    double lat = position.first;
    double lon = position.second;
    const double R = 6371000;
    double distance = speed * 0.5;
    double bearingRad = toRadians(bearing);
    double latRad = toRadians(lat);
    double lonRad = toRadians(lon);
    double newLat = asin(sin(latRad) * cos(distance / R) + cos(latRad) * sin(distance / R) * cos(bearingRad));
    double newLon = lonRad + atan2(sin(bearingRad) * sin(distance / R) * cos(latRad), cos(distance / R) - sin(latRad) * sin(newLat));
    newLat = toDegrees(newLat);
    newLon = toDegrees(newLon);
    return {newLat, newLon};
}

// Function to calculate initial bearing
double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2) {
    double dLon = toRadians(lon2 - lon1);
    double y = sin(dLon) * cos(toRadians(lat2));
    double x = cos(toRadians(lat1)) * sin(toRadians(lat2)) - sin(toRadians(lat1)) * cos(toRadians(lat2)) * cos(dLon);
    double initialBearing = atan2(y, x);
    return fmod(toDegrees(initialBearing) + 360, 360);
}

// Function to calculate distance
double calculateDistance(const std::pair<double, double> &myPosition, const std::pair<double, double> &targetPosition, double myAltitude) {
    double myLat = myPosition.first;
    double myLon = myPosition.second;
    double tarLat = targetPosition.first;
    double tarLon = targetPosition.second;
    const double R = 6371000;
    double dLat = toRadians(tarLat - myLat);
    double dLon = toRadians(tarLon - myLon);
    double haversineFormulaComponent = sin(dLat / 2) * sin(dLat / 2) +
                                       cos(toRadians(myLat)) * cos(toRadians(tarLon)) *
                                           sin(dLon / 2) * sin(dLon / 2);
    double centralAngle = 2 * atan2(sqrt(haversineFormulaComponent), sqrt(1 - haversineFormulaComponent));
    double distance = R * centralAngle;
    distance = sqrt(pow(distance, 2) + pow(myAltitude, 2));
    return distance;
}

#endif