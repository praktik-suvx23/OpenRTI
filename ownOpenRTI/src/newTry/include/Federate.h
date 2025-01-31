#ifndef FEDERATE_H
#define FEDERATE_H

#include "fedAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

Federate::Federate() {
}
public:

    rti1516e::RTIambassador* rtiAmbassador;
    FederateAmbassador* fedAmb;

    rti1516e::ObjectInstanceHandle vehicleInstanceHandle;
    rti1516e::ObjectClassHandle vehicleClassHandle;
    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;

    double position;
    double speed;

    Federate();
    ~Federate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void run();        
    void finalize();
    void resignFederation();
};

#endif // FEDERATE_H