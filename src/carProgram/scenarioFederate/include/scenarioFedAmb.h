#ifndef SCENARIOFEDAMB_H
#define SCENARIOFEDAMB_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include "../../include/MyPositionDecoder.h"
#include "../../include/MyFloat32Decoder.h"
#include "../../include/ObjectInstanceHandleHash.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

class scenarioFedAmb : public rti1516e::NullFederateAmbassador {
public:
    scenarioFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~scenarioFedAmb();

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

    // Interaction Class Handles
    rti1516e::InteractionClassHandle loadScenarioHandle;
    rti1516e::InteractionClassHandle scenarioLoadedHandle;
    rti1516e::InteractionClassHandle scenarioLoadFailureHandle;
    rti1516e::InteractionClassHandle startHandle;
    rti1516e::InteractionClassHandle stopHandle;

    // Parameter Handles for LoadScenario
    rti1516e::ParameterHandle scenarioNameParam;
    rti1516e::ParameterHandle initialFuelParam;

    // Parameter Handles for ScenarioLoaded
    rti1516e::ParameterHandle federateNameParam_Loaded;

    // Parameter Handles for ScenarioLoadFailure
    rti1516e::ParameterHandle federateNameParam_Failed;
    rti1516e::ParameterHandle errorMessageParam;

    // Parameter Handles for Start
    rti1516e::ParameterHandle timeScaleFactorParam;

    bool federateConnectedSuccessfully = false;
    bool federateFailedToConnect = false;
private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif