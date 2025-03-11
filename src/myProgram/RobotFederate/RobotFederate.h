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
    RobotFederate(int instance);
    ~RobotFederate();
    void runFederate(const std::wstring& federateName);

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyFederateAmbassador> federateAmbassador;

    void createRTIAmbassador(int instance);
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void subscribeInteractions();
    void publishInteractions();
    void runSimulationLoop();
    
    void assignToShip();
    void sendHitEvent();
    void resignFederation();

    void enableTimeManagement();

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    void initializeTimeFactory();

private:

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speedDis;
    std::wstring targetFederateName;
};

#endif