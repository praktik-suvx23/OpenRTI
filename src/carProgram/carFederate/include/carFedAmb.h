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

    // Handles
    rti1516e::ObjectClassHandle carObjectClassHandle;
    rti1516e::ObjectInstanceHandle carObjectInstanceHandle;
    rti1516e::AttributeHandle carNameAttribute;
    rti1516e::AttributeHandle licensePlateAttribute;
    rti1516e::AttributeHandle fuelLevelAttribute;
    rti1516e::AttributeHandle fuelTypeAttribute;
    rti1516e::AttributeHandle positionAttribute;
    // Interaction class handles
    rti1516e::InteractionClassHandle scenarioLoadFailureHandle;
    rti1516e::InteractionClassHandle scenarioLoadedHandle;
    rti1516e::InteractionClassHandle loadScenarioHandle;
    rti1516e::InteractionClassHandle startHandle;
    rti1516e::InteractionClassHandle stopHandle;
    // Parameter handles
    rti1516e::ParameterHandle scenarioNameParam;
    rti1516e::ParameterHandle initialFuelParam;
    rti1516e::ParameterHandle federateNameParam;
    rti1516e::ParameterHandle loadErrorMessageParam;
    rti1516e::ParameterHandle timeScaleFactorParam;

    std::string scenarioFilePath = "";
    double initialFuel = 10;
    double timeScaleFactor = 1;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif