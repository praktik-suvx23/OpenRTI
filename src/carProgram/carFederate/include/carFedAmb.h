#ifndef CARFEDAMB_H
#define CARFEDAMB_H

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

class carFedAmb : public rti1516e::NullFederateAmbassador {
public:
    carFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~carFedAmb();

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

    rti1516e::ObjectInstanceHandle carObjectHandle;
    rti1516e::AttributeHandle carNameHandle;
    rti1516e::AttributeHandle licensePlateHandle;
    rti1516e::AttributeHandle fuelLevelHandle;
    rti1516e::AttributeHandle fuelTypeHandle;
    rti1516e::AttributeHandle positionLatHandle;
    rti1516e::AttributeHandle positionLongHandle;
    rti1516e::InteractionClassHandle loadScenarioHandle;
    rti1516e::InteractionClassHandle startHandle;
    rti1516e::InteractionClassHandle stopHandle;
    rti1516e::ObjectInstanceHandle carObjectInstance;

    int savedData = 0;

    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif