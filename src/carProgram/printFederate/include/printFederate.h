#ifndef MASTFEDERATE_H
#define MASTFEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

#include "printFedAmb.h"

class mastFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<mastFedAmb> fedAmb;

    std::wstring federationName;
    std::wstring fomModule;
    std::vector<std::wstring> fomModules;
    std::wstring mimModule;

    std::wstring syncPoint = L"";

    mastFederate();
    ~mastFederate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation(std::wstring federateName);
    void achiveSyncPoint();
    void initializeHandles();
    void run();
    void finalize();
    void resignFederation();
};

#endif