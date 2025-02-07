#ifndef PUBFEDAMB_H
#define PUBFEDAMB_H

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

class pubFedAmb : public rti1516e::NullFederateAmbassador {
public:
    pubFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~pubFedAmb();

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    bool syncPointRegistered = false;

    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif