#ifndef MISSILECREATOR_H
#define MISSILECREATOR_H

#include "MissileCreatorFederateAmbassador.h"
#include "../include/loggingFunctions.h"

class MissileCreatorFederate {
public:
    MissileCreatorFederate();
    ~MissileCreatorFederate();
    void startMissileCreator();
private: 
    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void subscribeInteractions();
    void readyCheck();
    void runSimulationLoop();
    void resignFederation();

    std::wstring federateName = L"MissileCreatorFederate_" + std::to_wstring(getpid());
    std::wstring federationName = L"Federation";
    std::vector<std::wstring> fomModules = {L"" FOM_PATH};
    std::wstring mimModule = L"" MIM_PATH;

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MissileCreatorFederateAmbassador> federateAmbassador;
};

#endif // MISSILECREATOR_H