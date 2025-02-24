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

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyFederateAmbassador> federateAmbassador;

    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void subscribeInteractions();
    void publishInteractions();
    void runSimulationLoop();
    void sendHitEvent();
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