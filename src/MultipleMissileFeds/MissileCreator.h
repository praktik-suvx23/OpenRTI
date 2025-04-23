#ifndef MISSILECREATOR_H
#define MISSILECREATOR_H

#include "MissileCreatorFederateAmbassador.h"

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
    void subscribeAttributes();
    void publishAttributes();
    void subscribeInteractions();
    void publishInteractions();
    void waitForSetupSync();
    void readyCheck();
    void runSimulationLoop();
    void resignFederation();

    std::wstring federateName = L"MissileCreatorFederate";
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MissileCreatorFederateAmbassador> federateAmbassador;
    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
};

#endif // MISSILECREATOR_H