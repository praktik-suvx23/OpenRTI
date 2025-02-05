#ifndef MASTERFEDAMBASSADOR_H
#define MASTERFEDAMBASSADOR_H

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

class masterFederate;

class masterFedAmbassador : public rti1516e::NullFederateAmbassador {
public:
    masterFedAmbassador(masterFederate& fed);
    virtual ~masterFedAmbassador();

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

void synchronizationPointRegistrationSucceeded(const std::wstring& label) override;
void synchronizationPointRegistrationFailed(const std::wstring& label, rti1516e::SynchronizationPointFailureReason reason) override;

private:
    masterFederate& federate;
};

#endif // FEDERATEAMBASSADOR_H