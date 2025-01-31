#ifndef FEDERATEAMBASSADOR_H
#define FEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sstream>

class Federate;

class FederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    FederateAmbassador(Federate& fed);
    virtual ~FederateAmbassador();

    // Callback function for attribute updates
    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle objectHandle,
        const rti1516e::AttributeHandleValueMap& attributes,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType type,
        const rti1516e::LogicalTime& time,
        const rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle theHandle,
        rti1516e::SupplementalReflectInfo theReflectInfo
    ) override;

private:
    Federate& federate;

    double position;
    double speed;
};

#endif