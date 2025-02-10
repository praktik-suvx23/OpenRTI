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

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;
    std::wstring syncLabel = L"";

    rti1516e::InteractionClassHandle loadScenarioHandle;
    rti1516e::InteractionClassHandle scenarioLoadedHandle;

    rti1516e::ParameterHandle scenarioNameParamHandle;
    rti1516e::ParameterHandle topLeftLatParamHandle;
    rti1516e::ParameterHandle topLeftLongParamHandle;
    rti1516e::ParameterHandle bottomRightLatParamHandle;
    rti1516e::ParameterHandle bottomRightLongParamHandle;
    rti1516e::ParameterHandle startLatParamHandle;
    rti1516e::ParameterHandle startLongParamHandle;
    rti1516e::ParameterHandle stopLatParamHandle;
    rti1516e::ParameterHandle stopLongParamHandle;

    double scenarioTopLeftLat = 0.0;
    double scenarioTopLeftLong = 0.0;
    double scenarioBotRightLat = 0.0;
    double scenarioBotRightLong = 0.0;
    double scenarioStartLat = 0.0;
    double scenarioStartLong = 0.0;
    double scenarioEndLat = 0.0;
    double scenarioEndLong = 0.0;

    

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif