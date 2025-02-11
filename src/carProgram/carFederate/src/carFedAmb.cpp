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
        std::cout << "Received LoadScenario interaction." << std::endl;

        // Extract parameterValues for start position, goal position, and fuel level
        auto startLatIter = parameterValues.find(startLatHandle);
        auto startLongIter = parameterValues.find(startLongHandle);
        auto stopLatIter = parameterValues.find(stopLatHandle);
        auto stopLongIter = parameterValues.find(stopLongHandle);
        auto fuelIter = parameterValues.find(initFuelHandle);

        if (startLatIter != parameterValues.end() && startLongIter != parameterValues.end()) {
            std::memcpy(&startLat, startLatIter->second.data(), sizeof(double));
            std::memcpy(&startLong, startLongIter->second.data(), sizeof(double));
            std::cout << "Car start position set to (" << startLat << ", " << startLong << ")" << std::endl;
        }

        if (stopLatIter != parameterValues.end() && stopLongIter != parameterValues.end()) {
            std::memcpy(&goalLat, stopLatIter->second.data(), sizeof(double));
            std::memcpy(&goalLong, stopLongIter->second.data(), sizeof(double));
            std::cout << "Car goal position set to (" << goalLat << ", " << goalLong << ")" << std::endl;
        }

        if (fuelIter != parameterValues.end()) {
            std::memcpy(&fuelLevel, fuelIter->second.data(), sizeof(double));
            std::cout << "Initial fuel level set to: " << fuelLevel << std::endl;
        }
    } 
    else if (interactionClassHandle == startHandle) {
        std::cout << "Simulation started!" << std::endl;
        simulationRunning = true;
    } 
    else if (interactionClassHandle == stopHandle) {
        std::cout << "Simulation stopped!" << std::endl;
        simulationRunning = false;
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