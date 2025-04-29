#ifndef MISSILEFEDERATE_H
#define MISSILEFEDERATE_H

#include "MissileFederateAmbassador.h"
#include "../VisualRepresentation/SendData.cpp"
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
    void readyCheck();
    void runSimulationLoop();

    void SetupNewMissile(
        Missile missile,
        const rti1516e::HLAfloat64Time& logicalTime,
        const rti1516e::HLAfloat64Time& stepsize
    );
    void MissileFlightLoop(
        Missile& missile,
        const rti1516e::HLAfloat64Time& logicalTime,
        const rti1516e::HLAfloat64Time& stepsize
    );
    void resignFederation();

    void setupMissileVisualization();
    void sendTargetHitInteraction(Missile& missile, const rti1516e::LogicalTime& logicalTime);

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MissileFederateAmbassador> federateAmbassador;

    std::wstring federateName = L"MissileManagerFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> speedDis;
    std::wstring targetFederateName;
    rti1516e::HLAfloat64Interval lookAhead;
    // For socket communication
    int client_socket;
};

#endif // MISSILEFEDERATE_H