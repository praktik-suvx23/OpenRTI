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
    void prepareMissileLaunch(const rti1516e::LogicalTime& logicalTime, const int fireAmount, const double distance, const Ship& ship, const Ship& targetShip);
    void fireMissile(const rti1516e::LogicalTime& logicalTime, int fireAmount, const Ship& ship, const Ship& targetShip);
    void waitForExitLoop(double, double);
    void resignFederation();

    void logDetectionStart(const ShipTeam team);
    void detectEnemiesForShip(const Ship* ownShip, const std::vector<Ship*>& enemyShips, const double maxTargetDistance);
    void detectEnemies(double maxTargetDistance);

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    rti1516e::InteractionClassHandle tempConfirmHandshake;
    rti1516e::InteractionClassHandle tempInitiateHandshake;

    std::unordered_map<Ship*, int> tempLockingCount;
    
    std::wstring federateName = L"ShipFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";
};

#endif