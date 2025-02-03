#ifndef FEDERATE_H
#define FEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

// Forward declaration to avoid circular dependency
class FederateAmbassador;

class Federate {

public:

    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<FederateAmbassador> fedAmb;

    rti1516e::VariableLengthData tag; // Empty tag
    rti1516e::AttributeHandleSet attributeHandleSet;
    rti1516e::AttributeHandleValueMap attributeValues;
    rti1516e::InteractionClassHandle interactionClassHandle; //Not in use

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
    void initializeHandles(double position, double speed);
    void publishOnly();
    void subscribeOnly();
    void run();
    void updateAttributes(double position, double speed);
    void printCurrentSubscribedValues();
    void finalize();
    void resignFederation();
};

#endif // FEDERATE_H