#ifndef ADMINFEDERATE_H
#define ADMINFEDERATE_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include <algorithm>
#include <chrono>
#include <vector>
#include <map>

#include "AdminFederateAmbassador.h"
#include "../VisualRepresentation/SendData.cpp"

class AdminFederate {
public:
    AdminFederate();
    ~AdminFederate();
    void runFederate();
    
private:
    void createRTIambassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void registerSyncPoint();
    void achiveSyncPoint();
    void initializeHandles();
    void publishInteractions();
    void subscribeInteractions();
    void setupSimulation();
    void publishSetupSimulationInteraction(int teamA, int teamB, double timeScaleFactor);
    void registerSyncSimulationSetupComplete();
    void initializeTimeFactory();
    void enableTimeManagement();
    void socketsSetup();
    void readyCheck();
    void adminLoop();
    void resignFederation();

    void flushInitialHandshake(
        std::vector<InitialHandshake>& initialVector,
        std::vector<ConfirmHandshake>& confirmVector);
    void processInitialHandshake(
        std::vector<InitialHandshake>& initialVector,
        std::vector<ConfirmHandshake>& confirmVector);
    void flushConfirmHandshake(const rti1516e::LogicalTime& logicalTime, std::vector<ConfirmHandshake>& vector, Team side);
    void processConfirmHandshake(const rti1516e::LogicalTime& logicalTime, std::vector<ConfirmHandshake>& confirmVector, Team side);

    std::vector<ConfirmHandshake> blueConfirmHandshakeVector;
    std::vector<ConfirmHandshake> redConfirmHandshakeVector;

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<AdminFederateAmbassador> federateAmbassador;

    std::unordered_map<std::wstring, int32_t> missilesAssignedPerTarget;

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;

    std::wstring federateName = L"AdminFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring minModule = L"foms/MIM.xml";

    int redShips;
    int blueShips;
    double timeScaleFactor;

    int heartbeat_socket;
};
    
#endif