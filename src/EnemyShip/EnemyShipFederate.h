#ifndef ENEMYSHIPFEDERATE_H
#define ENEMYSHIPFEDERATE_H

#include "EnemyShipFederateAmbassador.h"

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

class EnemyShipFederate {
public: 
    EnemyShipFederate();
    ~EnemyShipFederate();
    void runFederate(const std::wstring& federateName);

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<EnemyShipFederateAmbassador> federateAmbassador;

    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void subscribeInteractions();
    void waitForSetupSync();
    void publishInteractions();
    void runSimulationLoop();

    void readJsonFile();
    void publishAttributes();
    void registerShipObject(const int& amountOfShips);

    void updateShipAttributes(
        const std::wstring& shipLocation, 
        const std::wstring& futureShipLocation, 
        double shipSpeed, 
        const rti1516e::LogicalTime& logicalTimePtr
    );

    void sendInteraction(const rti1516e::LogicalTime& logicalTime, int fireAmount, std::wstring targetName);

    void enableTimeManagement();
    void resignFederation();


    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    void initializeTimeFactory();

    std::wstring federateName = L"EnemyShipFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

};


#endif