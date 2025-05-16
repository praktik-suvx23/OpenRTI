#ifndef PYLINK_H
#define PYLINK_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <vector>
#include <map>

#include "PyLinkAmbassador.h"
#include "../SendData.cpp"
#include "../../include/decodePosition.h"

class PyLink {
public:
    PyLink();
    ~PyLink();
    void runFederate();
    
private:
    void createRTIambassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeAttributes();
    void initializeTimeFactory();
    void enableTimeManagement();
    void socketsSetup();
    void readyCheck();
    void communicationLoop();
    void resignFederation();

    void logShip(Ship& ship);
    void logMissile(Missile& missile);

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<PyLinkAmbassador> federateAmbassador;

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;

    std::wstring federateName = L"PyLink";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring minModule = L"foms/MIM.xml";

    double timeScaleFactor;

    int blueship_socket;
    int redship_socket;
    int missile_socket;
};
    
#endif