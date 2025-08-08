#ifndef MISSILECALCULATOR_H
#define MISSILECALCULATOR_H

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

// Function declarations
std::wstring stringToWString(const std::string& str);
std::pair<double, double> stringToPair(const std::string& str);
double getAltitude(double distanceToTarget, double currentDistanceTraveled);
double getFuelLevel(double speed); //Not being used for the moment
double getSpeed(double cSpeed, double minSpeed, double maxSpeed);
double toRadians(double degrees);
double toDegrees(double radians);
double increaseAltitude(double altitude, double speed, double distance);
double calculateYBearing(double MissileAltitude, double groundDistance, double targetAltitude);
double reduceAltitude(double altitude, double speed, double distance);
std::pair<double, double> calculateNewPosition(const std::pair<double, double> &position, double speed, double bearing);
double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2);
double calculateDistance(const std::pair<double, double> &myPosition, const std::pair<double, double> &targetPosition, double myAltitude);

#endif // MISSILECALCULATOR_H