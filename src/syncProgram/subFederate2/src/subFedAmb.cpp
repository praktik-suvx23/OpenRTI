#include "../include/subFedAmb.h"
#include "../include/subFederate.h"

subFedAmb::subFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

subFedAmb::~subFedAmb() {}

void subFedAmb::receiveInteraction(
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
            int value = parameterData.get();
            if(value % 10 == 0){
                std::wcout << L"Received InteractionClass1 with Parameter1: " << value << std::endl;
                savedData = value;
            }
        }
    }
}

void subFedAmb::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }

    if (label == L"ShutdownSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
        syncLabel = label; 
    }
}