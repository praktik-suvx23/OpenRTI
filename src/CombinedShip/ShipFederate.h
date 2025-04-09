#ifndef SHIPFEDERATE_H
#define SHIPFEDERATE_H

#include "ShipFederateAmbassador.h"
#include "../VisualRepresentation/SendData.cpp"
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

class ShipFederate {
public: 
    ShipFederate();
    ~ShipFederate();
    void startShip(int team);

private:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyShipFederateAmbassador> federateAmbassador;

    void createRTIAmbassador();
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
    void createShipsSyncPoint();
    void initializeTimeFactory();
    void enableTimeManagement();
    void runSimulationLoop();
    void readyCheck();
    void sendInteraction(const rti1516e::LogicalTime& logicalTime, int fireAmount, const Ship& ship, const Ship& targetShip);    
    void resignFederation();

    void setupMissileVisualization();

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    
    std::wstring federateName = L"ShipFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

    int client_socket;
};

#endif