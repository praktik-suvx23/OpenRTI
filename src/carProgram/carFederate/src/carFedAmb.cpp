#include "../include/carFedAmb.h"
#include "../include/carFederate.h"

carFedAmb::carFedAmb(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

carFedAmb::~carFedAmb() {}

void carFederate::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClass,
    const rti1516e::ParameterHandleValueMap& parameters,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportation,
    rti1516e::OrderType receivedOrder) {

    if (interactionClass == loadScenarioHandle) {
        std::cout << "Received LoadScenario interaction." << std::endl;
        // Reset car fuel level & position
        fuelLevelHandle = 100;  // Example initial value
        fedAmb->position = {fedAmb->startLat, fedAmb->startLong};
    } 
    else if (interactionClass == fedAmb->startHandle) {
        std::cout << "Simulation started!" << std::endl;
        fedAmb->simulationRunning = true;
    } 
    else if (interactionClass == fedAmb->stopHandle) {
        std::cout << "Simulation stopped!" << std::endl;
        fedAmb->simulationRunning = false;
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