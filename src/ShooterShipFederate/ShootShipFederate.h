#ifndef SHOOTSHIPFEDERATE_H
#define SHOOTSHIPFEDERATE_H

#include "ShootShipFederateAmbassador.h"

#include <iostream>
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

class ShootShipFederate {
public: 
    ShootShipFederate();
    ~ShootShipFederate();
    void startShootShip();

private:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyShootShipFederateAmbassador> federateAmbassador;

    void createRTIAmbassador();
    void readJsonFile();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void publishAttributes();
    void subscribeAttributes();
    void publishInteractions();
    void subscribeInteractions();
    void waitForSetupSync();
    void initializeTimeFactory();
    void enableTimeManagement();
    void runSimulationLoop();
    void sendInteraction(const rti1516e::LogicalTime& logicalTime, int fireAmount, const Ship& ship);
    void resignFederation();

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    
    std::wstring federateName = L"ShootShipFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";
};

#endif