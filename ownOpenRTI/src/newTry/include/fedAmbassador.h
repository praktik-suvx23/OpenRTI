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

    double lastReceivedPosition = 0.0;
    double lastReceivedSpeed = 0.0;

    // Callback function for attribute updates
virtual void reflectAttributeValues (
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const & theAttributeValues,
    rti1516e::VariableLengthData const & theUserSuppliedTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::MessageRetractionHandle theHandle,
    rti1516e::SupplementalReflectInfo theReflectInfo
) RTI_THROW ((FederateInternalError));

virtual void provideAttributeValueUpdate(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleSet const & theAttributes,
    rti1516e::VariableLengthData const & theTag
) RTI_THROW ((FederateInternalError));

private:
    Federate& federate;

    double position = 0.0;
    double speed = 0.0;
};

#endif // FEDERATEAMBASSADOR_H