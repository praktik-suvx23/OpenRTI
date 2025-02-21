#ifndef ROBOT_H
#define ROBOT_H
#pragma once
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>
#include <cmath>
#include <iomanip>

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> speedDis;

class Robot
{
private:
    double currentSpeed = 0.0;
    double currentFuelLevel = 100.0;
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    std::wstring currentPosition = std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
    double currentAltitude = 0.0;

    // function declarations

    // Speed range: 0 to 100 units
public:
    std::wstring getPosition(double &currentLatitude, double &currentLongitude)
    {
        // Simulate position sensor reading (latitude, longitude)
        currentLatitude = 20.4382900;  // Increment latitude
        currentLongitude = 15.6253400; // Increment longitude
        return std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
    }

    double getAltitude()
    {
        // Simulate altitude sensor reading with oscillation
        static double altitude = 50.0;
        static bool increasing = true;
        static double angle = 60.0; // Angle in degrees

        if (increasing)
        {
            altitude += 50.0 * sin(angle * M_PI / 180); // Increase altitude
            if (altitude > 1000.0)
            {
                altitude = 1000.0;
                increasing = false;
            }
        }
        else
        {
            altitude -= 50.0 * sin(angle * M_PI / 180); // Decrease altitude
            if (altitude < 0.0)
            {
                altitude = 0.0;
                increasing = true;
            }
        }
        return altitude;
    }

    double getFuelLevel(double speed)
    {
        // Simulate fuel level sensor reading
        static double fuelLevel = 100.0;
        fuelLevel -= 0.01 * (speed / 100); // Decrease fuel level
        if (fuelLevel < 0)
        {
            fuelLevel = 0;
        }
        return fuelLevel;
    }

    double getSpeed(double cSpeed, double minSpeed, double maxSpeed)
    {
        // Update the range of the speed distribution based on the current speed
        speedDis.param(std::uniform_real_distribution<>::param_type(cSpeed - 10.0, cSpeed + 10.0));
        // Generate a new random speed value within the updated range
        double newSpeed = speedDis(gen);
        // Clamp the speed value to ensure it stays within the specified range
        if (newSpeed < minSpeed)
        {
            newSpeed = minSpeed;
        }
        else if (newSpeed > maxSpeed)
        {
            newSpeed = maxSpeed;
        }
        return newSpeed;
    }

    std::vector<std::wstring> split(const std::wstring &s, wchar_t delimiter)
    {
        std::vector<std::wstring> tokens;
        std::wstring token;
        std::wstringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    double toRadians(double degrees)
    {
        return degrees * M_PI / 180.0;
    }

    double toDegrees(double radians)
    {
        return radians * 180.0 / M_PI;
    }

    double reduceAltitude(double altitude, double speed, double distance) {
        double newAltitude = 0.0;
    
        // Check for zero distance to avoid division by zero
        if (distance == 0) {
            return altitude; // or handle this case as needed
        }
    
        // Ensure the argument to asin is within the valid range [-1, 1]
        double ratio = altitude / distance;
        if (ratio < -1.0) {
            ratio = -1.0;
        } else if (ratio > 1.0) {
            ratio = 1.0;
        }
    
        if (altitude <= 200 && distance > 250) {
            newAltitude = 200;
        } else {
            newAltitude = (distance - speed * 0.1) * sin(asin(ratio));
            if (newAltitude < 0) {
                newAltitude = 0;
            }
        }
    
        return newAltitude;
    }

    std::wstring calculateNewPosition(const std::wstring &position, double speed, double bearing)
    {
        std::vector<std::wstring> tokens = split(position, L',');
        if (tokens.size() != 2)
        {
            throw std::invalid_argument("Invalid position format");
        }

        double lat = std::stod(tokens[0]);
        double lon = std::stod(tokens[1]);

        const double R = 6371000; // Radius of the Earth in meters

        double distance = speed * 0.1;          // Distance traveled in meters (since speed is in m/s and time is 1 second) change according to time
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

    double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2)
    {
        double dLon = toRadians(lon2 - lon1);
        double y = sin(dLon) * cos(toRadians(lat2));
        double x = cos(toRadians(lat1)) * sin(toRadians(lat2)) - sin(toRadians(lat1)) * cos(toRadians(lat2)) * cos(dLon);
        double initialBearing = atan2(y, x);
        return fmod(toDegrees(initialBearing) + 360, 360);
    }

    double calculateInitialBearingWstring(const std::wstring &position1, const std::wstring &position2)
    {
        std::vector<std::wstring> tokens1 = split(position1, L',');
        std::vector<std::wstring> tokens2 = split(position2, L',');

        if (tokens1.size() != 2 || tokens2.size() != 2)
        {
            throw std::invalid_argument("Invalid position format");
        }

        double lat1 = std::stod(tokens1[0]);
        double lon1 = std::stod(tokens1[1]);
        double lat2 = std::stod(tokens2[0]);
        double lon2 = std::stod(tokens2[1]);

        return calculateInitialBearingDouble(lat1, lon1, lat2, lon2);
    }

    // Function to calculate distance between two positions given as wstrings
    // Input value format: latitude,longitude (e.g., L"20.43829,15.62534")
    double calculateDistance(const std::wstring &position1, const std::wstring &position2, double currentAltitude)
    {
        std::vector<std::wstring> tokens1 = split(position1, L','); // Used to split the latitude and longitude of position 1
        std::vector<std::wstring> tokens2 = split(position2, L','); // Used to split the latitude and longitude of position 2

        if (tokens1.size() != 2 || tokens2.size() != 2)
        {
            throw std::invalid_argument("Invalid position format");
        }

        double lat1 = std::stod(tokens1[0]);
        double lon1 = std::stod(tokens1[1]);
        double lat2 = std::stod(tokens2[0]);
        double lon2 = std::stod(tokens2[1]);

        const double R = 6371000;             // Radius of the Earth in meters (average value)
        double dLat = toRadians(lat2 - lat1); // Delta latitude
        double dLon = toRadians(lon2 - lon1); // Delta longitude
        double haversineFormulaComponent = sin(dLat / 2) * sin(dLat / 2) +
                                           cos(toRadians(lat1)) * cos(toRadians(lat2)) *
                                               sin(dLon / 2) * sin(dLon / 2);
        double centralAngle = 2 * atan2(sqrt(haversineFormulaComponent), sqrt(1 - haversineFormulaComponent));
        double distance = R * centralAngle;
        distance = sqrt(pow(distance, 2) + pow(currentAltitude, 2));
        return distance; // In meters
    }
};

#endif