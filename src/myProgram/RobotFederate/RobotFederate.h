/*
Define the RobotFederate class.
Declare the constructor and destructor.
Declare the runFederate method.
Declare methods for creating and connecting the RTI ambassador.
Declare methods for initializing the federation, joining the federation, and waiting for sync points.
Declare methods for initializing handles, subscribing to attributes, running the simulation loop, and resigning from the federation.
Declare methods for robot functionality (e.g., getPosition, getAltitude, getFuelLevel, getSpeed, split, toRadians, toDegrees, reduceAltitude, calculateNewPosition, calculateInitialBearingDouble, calculateInitialBearingWstring, calculateDistance).
Declare private member variables for robot attributes (e.g., currentSpeed, currentFuelLevel, currentLatitude, currentLongitude, currentPosition, currentAltitude).
*/
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