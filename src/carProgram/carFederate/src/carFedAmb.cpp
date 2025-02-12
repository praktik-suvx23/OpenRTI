#include "../include/carFedAmb.h"
#include "../include/carFederate.h"
#include <cstring>

carFedAmb::carFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

carFedAmb::~carFedAmb() {}

void carFedAmb::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    if (interactionClassHandle == loadScenarioHandle) {
        
    }
}

void carFedAmb::announceSynchronizationPoint(
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