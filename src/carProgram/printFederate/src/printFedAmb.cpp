#include "../include/printFederate.h"
#include "../include/printFedAmb.h"
#include <iostream>
#include <cstring>

mastFedAmb::mastFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

mastFedAmb::~mastFedAmb() {}

void mastFedAmb::announceSynchronizationPoint (
     std::wstring  const & label,
     rti1516e::VariableLengthData const & theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Master Federate synchronized at InitialSync." << std::endl;
        syncLabel = label;
    }

    if (label == L"ShutdownSync") {
        std::wcout << L"Master Federate synchronized at ShutdownSync." << std::endl;
        syncLabel = label;
    }
}

void mastFedAmb::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::LogicalTime const & time,
    rti1516e::OrderType receivedOrder,
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