#ifndef ADMINFEDERATE_H
#define ADMINFEDERATE_H

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
    std::wstring federationName = L"Federation";
    std::vector<std::wstring> fomModules = {L"" FOM_PATH};
    std::wstring minModule = L"" MIM_PATH;

    int redShips;
    int blueShips;
    double timeScaleFactor;
};
    
#endif