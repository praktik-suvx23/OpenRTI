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
    ShootShipFederate(int instance);
    ~ShootShipFederate();
    void runFederate(const std::wstring& federateName);

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyShootShipFederateAmbassador> federateAmbassador;

    void createRTIAmbassador(int instance);
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void subscribeInteractions(); //Not used for the moment
    void publishInteractions();
    void runSimulationLoop();

    void sendInteraction(const rti1516e::LogicalTime& logicalTime);

    void enableTimeManagement();
    void resignFederation();


    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    void initializeTimeFactory();

};


#endif