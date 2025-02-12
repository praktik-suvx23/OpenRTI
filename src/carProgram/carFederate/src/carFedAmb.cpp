#include "../include/carFedAmb.h"
#include "../include/carFederate.h"
#include <cstring>
#include <locale>
#include <codecvt>

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
        auto iter = parameterValues.find(scenarioNameParam);
        if (iter != parameterValues.end()) {
            rti1516e::HLAunicodeString scenarioNameDecoder;
            scenarioNameDecoder.decode(iter->second);
            std::wstring tempScenarioFilePath = scenarioNameDecoder.get();
            std::wcout << L"Received scenario file path: " << tempScenarioFilePath << std::endl;

            // Convert wstring to string for std::ifstream
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            scenarioFilePath = converter.to_bytes(tempScenarioFilePath);
        }

        auto iterFuel = parameterValues.find(initialFuelParam);
        if (iterFuel != parameterValues.end()) {
            rti1516e::HLAinteger32BE fuelDecoder;
            fuelDecoder.decode(iterFuel->second);
            initialFuel = fuelDecoder.get();
            std::cout << "Received initial fuel: " << initialFuel << std::endl;
        }
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

    if (label == L"ScenarioLoaded") {
        std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
        syncLabel = label; 
    }
}