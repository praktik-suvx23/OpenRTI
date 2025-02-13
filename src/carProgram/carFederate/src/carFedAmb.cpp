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
    std::cout << "[DEBUG] FROM carFedAmb" << std::endl;
    if (interactionClassHandle == loadScenarioHandle) {
        std::cout << "Received LoadScenario interaction." << std::endl;
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

    else if (interactionClassHandle == startHandle) {
        std::cout << "Received Start interaction." << std::endl;
        auto iterTimeScale = parameterValues.find(timeScaleFactorParam);
        if (iterTimeScale != parameterValues.end()) {
            rti1516e::HLAfloat32BE timeScaleDecoder;  
            timeScaleDecoder.decode(iterTimeScale->second);
            timeScaleFactor = timeScaleDecoder.get();
            std::cout << "Received time scale factor: " << timeScaleFactor << std::endl;
        }
    }
}

void carFedAmb::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }

    if (label == L"ShutdownSync") {
        std::wcout << L"Federate received synchronization announcement: ShutdownSync." << std::endl;
        syncLabel = label; 
    }

    if (label == L"ScenarioLoaded") {
        std::wcout << L"Federate received synchronization announcement: ScenarioLoaded." << std::endl;
        syncLabel = label; 
    }
}