#include "MissileCalculator.h"

// External random number generator
extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> speedDis;

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

// Utility functions for conversions
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

double increaseAltitude(double altitude, double speed, double distance) {
    // Check for zero distance to avoid division by zero
    if (distance == 0) {
        return altitude;
    }
    std::wcout << L"Altitude before ascending: " << altitude << std::endl;

    double angle = 45.0; // Ascent rate in degrees
    double ascentDistance = distance - (speed * 0.5);
    
    altitude += 50.0 * sin(angle * M_PI / 180);
    std::wcout << L"Altitude after ascending: " << altitude << std::endl;

    return altitude;
}

// Function to reduce altitude
double reduceAltitude(double altitude, double speed, double distance) {

    // Check for zero distance to avoid division by zero
    if (distance == 0) {
        return altitude; 
    }
    std::wcout << L"Altitude before descending: " << altitude << std::endl;

    double angle = 45.0; // Descent rate in degrees
    double descentDistance = distance - (speed * 0.5);
    
    altitude -= 50.0 * sin(angle * M_PI / 180);
    std::wcout << L"Altitude after descending: " << altitude << std::endl;

    // Ensure the altitude does not go below zero
    if (altitude < 0) {
        altitude = 0;
        std::wcout << L"Altitude below 0 not allowed" << std::endl;
    }

    return altitude;
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