#ifndef MISSILEFEDERATE_H
#define MISSILEFEDERATE_H

#include "MissileFederateAmbassador.h"
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

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class MissileFederate {
public:
    MissileFederate();
    ~MissileFederate();
    void startMissileManager();

private:
    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void publishAttributes();
    void subscribeInteractions();
    void publishInteractions();
    void waitForSetupSync();
    void initializeTimeFactory();
    void enableTimeManagement();
    void runSimulationLoop();
    void resignFederation();

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MissileFederateAmbassador> federateAmbassador;

    std::wstring federateName = L"MissileManagerFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

    std::queue<int> missileQueue;           // If threads wont be used, remove.
    std::mutex queueMutex;                  // If threads wont be used, remove.
    std::condition_variable queueCondition; // If threads wont be used, remove.
    bool stopMissileSimulation = false;     // If threads wont be used, remove.

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speedDis;
    std::wstring targetFederateName;
};

#endif