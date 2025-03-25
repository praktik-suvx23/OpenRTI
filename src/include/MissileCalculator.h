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
std::wstring getPosition(double &currentLatitude, double &currentLongitude);
double getAltitude();
double getFuelLevel(double speed);
double getSpeed(double cSpeed, double minSpeed, double maxSpeed);
std::vector<std::wstring> split(const std::wstring &s, wchar_t delimiter);
double toRadians(double degrees);
double toDegrees(double radians);
double reduceAltitude(double altitude, double speed, double distance);
std::pair<double, double> calculateNewPosition(const std::pair<double, double> &position, double speed, double bearing);
double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2);
double calculateDistance(const std::pair<double, double> &myPosition, const std::pair<double, double> &targetPosition, double myAltitude);

#endif // MISSILECALCULATOR_H