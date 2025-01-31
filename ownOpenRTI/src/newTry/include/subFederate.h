#ifndef FEDERATE_H
#define FEDERATE_H

#include "fedAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

class Federate {
public:
    rti1516e::RTIambassador* rtiAmbassador;
    FederateAmbassador* fedAmb;

    rti1516e::InteractionClassHandle vehicleReadyHandle;

    rti1516e::ObjectInstanceHandle vehicleInstanceHandle;
    rti1516e::ObjectClassHandle vehicleClassHandle;
    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;

    Federate();
    ~Federate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void subscribeToAttributes();
    void run();        
    void finalize();
    void resignFederation();

    std::chrono::steady_clock::time_point lastUpdateTime;
    bool _done = false;
    int _state;
    std::unique_ptr<rti1516e::RTIambassador> _rtiAmbassador;
    std::unique_ptr<rti1516e::FederateAmbassador> _federateAmbassador;
};

#endif // FEDERATE_H