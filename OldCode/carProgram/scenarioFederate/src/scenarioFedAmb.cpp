#include "../include/scenarioFederate.h"
#include "../include/scenarioFedAmb.h"
#include <iostream>
#include <cstring>

scenarioFedAmb::scenarioFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

scenarioFedAmb::~scenarioFedAmb() {}

void scenarioFedAmb::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
        std::cout << "[DEBUG] scenarioFedAmb.cpp" << std::endl;
    if (interactionClassHandle == scenarioLoadedHandle) {
        std::cout << "[DEBUG] Scenario Loaded" << std::endl;
        // Scenario Loaded successfully
        auto iter = parameterValues.find(federateNameParam_Loaded);
        if (iter != parameterValues.end()) {
            rti1516e::HLAunicodeString federateNameDecoder;
            federateNameDecoder.decode(iter->second);
            std::wstring federateName = federateNameDecoder.get();
            federateConnectedSuccessfully = true;
            std::wcout << L"Scenario successfully loaded by federate: " << federateName << std::endl;
        }
    } else if (interactionClassHandle == scenarioLoadFailureHandle) {
        std::cout << "[DEBUG] Scenario Load Failed" << std::endl;
        // Scenario Load Failed
        auto iterName = parameterValues.find(federateNameParam_Failed);
        auto iterError = parameterValues.find(errorMessageParam);
        if (iterName != parameterValues.end() && iterError != parameterValues.end()) {
            rti1516e::HLAunicodeString federateNameDecoder;
            federateNameDecoder.decode(iterName->second);
            std::wstring federateName = federateNameDecoder.get();

            rti1516e::HLAunicodeString errorMessageDecoder;
            errorMessageDecoder.decode(iterError->second);
            std::wstring errorMessage = errorMessageDecoder.get();
            federateFailedToConnect = true;
            std::wcout << L"Federate " << federateName << L" failed to load scenario. Error: " << errorMessage << std::endl;
        }
    }
}

void scenarioFedAmb::announceSynchronizationPoint (
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

    if (label == L"ScenarioLoaded") {
        std::wcout << L"scenarioFederate synchronized at ScenarioLoaded." << std::endl;
        syncLabel = label;
    }
}
