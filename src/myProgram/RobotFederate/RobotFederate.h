#ifndef ROBOTFEDERATE_H
#define ROBOTFEDERATE_H

#include "RobotFederateAmbassador.h"
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

class RobotFederate {
public:
    RobotFederate();
    ~RobotFederate();
    void runFederate(const std::wstring& federateName);

    // Make these private?
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyFederateAmbassador> federateAmbassador;

    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void runSimulationLoop();
    void resignFederation();

    // Robot functionality
    std::wstring getPosition(double &currentLatitude, double &currentLongitude);
    double getAltitude();
    double getFuelLevel(double speed);
    double getSpeed(double cSpeed, double minSpeed, double maxSpeed);
    std::vector<std::wstring> split(const std::wstring &s, wchar_t delimiter);
    double toRadians(double degrees);
    double toDegrees(double radians);
    double reduceAltitude(double altitude, double speed, double distance);
    std::wstring calculateNewPosition(const std::wstring &position, double speed, double bearing);
    double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2);
    double calculateInitialBearingWstring(const std::wstring &position1, const std::wstring &position2);
    double calculateDistance(const std::wstring &position1, const std::wstring &position2, double currentAltitude);

private:
    double currentSpeed = 0.0;
    double currentFuelLevel = 100.0;
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    std::wstring currentPosition = std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
    double currentAltitude = 0.0;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speedDis;
};

#endif