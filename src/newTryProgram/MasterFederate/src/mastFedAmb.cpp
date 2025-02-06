#include "../include/mastFederate.h"
#include "../include/mastFedAmb.h"
#include <iostream>
#include <cstring>

mastFedAmb::mastFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

mastFedAmb::~mastFedAmb() {}

void mastFedAmb::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"receiveInteraction called" << std::endl;
    if (interactionClassHandle == interactionClassHandle1) {
        auto parameterHandle = parameterValues.find(parameterHandle1);
        if (parameterHandle != parameterValues.end()) {
            rti1516e::HLAinteger32BE parameterData;
            parameterData.decode(parameterHandle->second);
            std::wcout << L"Received InteractionClass1 with Parameter1: " << parameterData.get() << std::endl;
        }
    }
}