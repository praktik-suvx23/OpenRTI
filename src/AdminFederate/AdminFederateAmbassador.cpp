#include "AdminFederateAmbassador.h"
#include "AdminFederateAmbassadorHelper.h"

AdminFederateAmbassador::AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

AdminFederateAmbassador::~AdminFederateAmbassador() {}

void AdminFederateAmbassador::announceSynchronizationPoint (
     std::wstring  const & label,
     rti1516e::VariableLengthData const & theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Master Federate synchronized at InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"Master Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label == L"AdminReady") {
        std::wcout << L"Master Federate synchronized at AdminReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipReady") {
        std::wcout << L"Master Federate synchronized at ShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"EveryoneReady") {
        std::wcout << L"Master Federate synchronized at EveryoneReady." << std::endl;
        syncLabel = label;
    }
}

void AdminFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void AdminFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void AdminFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}