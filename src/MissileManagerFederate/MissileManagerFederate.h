#ifndef MISSILEMANAGER_H
#define MISSILEMANAGER_H

#include "MissileManagerFederateAmbassador.h"
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

class MissileManager {
public:
    MissileManager();
    ~MissileManager();
    void startMissileManager();
    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void subscribeInteractions();
    void initializeTimeFactory();
    void enableTimeManagement();
    void runSimulationLoop();
    void resignFederation();
    
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MissileManagerAmbassador> federateAmbassador;
    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speedDis;
};

#endif