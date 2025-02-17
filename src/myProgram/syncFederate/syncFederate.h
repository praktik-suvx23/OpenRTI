#ifndef SYNCFEDERATE_H
#define SYNCFEDERATE_H

#include "syncFederateAmbassador.h"

class syncFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<syncFederateAmbassador> fedAmb;

    std::wstring syncPoint = L"ReadyToRun";

    syncFederate();
    ~syncFederate();
    void runFederate();
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void registerSyncPoint();
    void achiveSyncPoint();
    void resignFederation();
};

#endif