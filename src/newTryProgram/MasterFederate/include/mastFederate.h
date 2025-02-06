#ifndef MASTFEDERATE_H
#define MASTFEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

#include "mastFedAmb.h"

class mastFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<mastFedAmb> fedAmb;

    rti1516e::VariableLengthData tag;
    rti1516e::AttributeHandleSet attributeHandleSet;
    rti1516e::AttributeHandleValueMap attributeValues;
    rti1516e::InteractionClassHandle interactionClassHandle;

    std::wstring federationName;

    mastFederate();
    ~mastFederate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation(std::wstring federateName);
    void registerSyncPoint();
    void achiveSyncPoint();
    void initializeHandles();
    void run();
    void finalize();
    void resignFederation();
};

#endif