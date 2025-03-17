#ifndef ADMINFEDERATE_H
#define ADMINFEDERATE_H

#include "AdminFederateAmbassador.h"

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
    void adminLoop();
    void resignFederation();

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<AdminFederateAmbassador> federateAmbassador;

    std::wstring federateName = L"AdminFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/robot.xml"};
    std::wstring minModule = L"foms/MIM.xml";
};
    
#endif