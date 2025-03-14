#ifndef ADMINFEDERATE_H
#define ADMINFEDERATE_H

#include "AdminFederateAmbassador.h"

class AdminFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<AdminFederateAmbassador> federateAmbassador;

    std::wstring syncPoint = L"ReadyToRun";

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
    void resignFederation();
};

#endif