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
    rti1516e::ParameterHandle scenarioInitialFuelAmountHandle;

    double initialFuelAmount = 0;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif