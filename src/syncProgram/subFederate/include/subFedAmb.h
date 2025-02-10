#ifndef SUBFEDAMB_H
#define SUBFEDAMB_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

class subFedAmb : public rti1516e::NullFederateAmbassador {
public:
    subFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~subFedAmb();

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    std::wstring syncLabel = L"";

    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif