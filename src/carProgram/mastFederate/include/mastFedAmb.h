#ifndef MASTFEDAMB_H
#define MASTFEDAMB_H

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

class mastFedAmb : public rti1516e::NullFederateAmbassador {
public:
    mastFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~mastFedAmb();

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;

    std::wstring syncLabel = L"";

// Initialize Car object class attributes
    rti1516e::ObjectClassHandle carObjectClassHandle;
// Attribute handles for Car object class
    rti1516e::AttributeHandle nameAttributeHandle;
    rti1516e::AttributeHandle licensePlateNumberAttributeHandle;
    rti1516e::AttributeHandle fuelLevelAttributeHandle;
    rti1516e::AttributeHandle fuelTypeAttributeHandle;
    rti1516e::AttributeHandle positionAttributeHandle;
// Initialize interaction class handles and parameters for LoadScenario interaction
    rti1516e::InteractionClassHandle loadScenarioClassHandle;
    rti1516e::ParameterHandle scenarioNameParamHandle;
    rti1516e::ParameterHandle initialFuelAmountParamHandle;
// Initialize interaction class handles and parameters for ScenarioLoaded interaction
    rti1516e::InteractionClassHandle scenarioLoadedClassHandle;
    rti1516e::ParameterHandle federateNameParamHandle;
// Initialize interaction class handles and parameters for ScenarioLoadFailure interaction
    rti1516e::InteractionClassHandle scenarioLoadFailureClassHandle;
    rti1516e::ParameterHandle federateNameFailureParamHandle;
    rti1516e::ParameterHandle errorMessageFailureParamHandle;
// Initialize interaction class handles and parameters for Start interaction
    rti1516e::InteractionClassHandle startClassHandle;
    rti1516e::ParameterHandle timeScaleFactorParamHandle;
// Initialize interaction class handle for Stop interaction (no parameters)
    rti1516e::InteractionClassHandle stopClassHandle;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif