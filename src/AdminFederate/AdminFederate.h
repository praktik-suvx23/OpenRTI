#ifndef ADMINFEDERATE_H
#define ADMINFEDERATE_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
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
    void setupSimulation();
    void publishSetupSimulationInteraction(int teamA, int teamB, double timeScaleFactor);
    void registerSyncSimulationSetupComplete();
    void initializeTimeFactory();
    void enableTimeManagement();
    void socketsSetup();
    void readyCheck();
    void adminLoop();
    void resignFederation();

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<AdminFederateAmbassador> federateAmbassador;

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;

    std::wstring federateName = L"AdminFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring minModule = L"foms/MIM.xml";

    int redShips;
    int blueShips;
    double timeScaleFactor;

    int receiveHeartbeat = -1;

    int heartbeat_socket;
};
    
#endif