#include "AdminFederateAmbassador.h"
#include "AdminFederateAmbassadorHelper.h"

AdminFederateAmbassador::AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

AdminFederateAmbassador::~AdminFederateAmbassador() {}

void AdminFederateAmbassador::announceSynchronizationPoint (
     std::wstring  const & label,
     rti1516e::VariableLengthData const & theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label == L"AdminReady") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at AdminReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipReady") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at ShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"EveryoneReady") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at EveryoneReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"ReadyToExit") {
        std::wcout << L"[SyncPoint] Master Federate synchronized at ReadyToExit." << std::endl;
        syncLabel = label;
    }
}

void AdminFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
}

void AdminFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
}

void AdminFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    isAdvancing = false;
}

std::chrono::time_point<std::chrono::high_resolution_clock> AdminFederateAmbassador::getStartTime() const {
    return startTime;
}
void AdminFederateAmbassador::setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time) {
    startTime = time;
}

// Initiate Handshake - RED
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassInitiateRedHandshake() const { return interactionClassInitiateRedHandshake; }
void AdminFederateAmbassador::setInteractionClassInitiateRedHandshake(const rti1516e::InteractionClassHandle& handle) { interactionClassInitiateRedHandshake = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedShooterID() const { return parameterHandleInitiateRedShooterID; }
void AdminFederateAmbassador::setParamInitiateRedShooterID(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedShooterID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedMissilesAvailable() const { return parameterHandleInitiateRedMissilesAvailable; }
void AdminFederateAmbassador::setParamInitiateRedMissilesAvailable(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedMissilesAvailable = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedTargetID() const { return parameterHandleInitiateRedTargetID; }
void AdminFederateAmbassador::setParamInitiateRedTargetID(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedTargetID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedMaxMissilesRequired() const { return parameterHandleInitiateRedMaxMissilesRequired; }
void AdminFederateAmbassador::setParamInitiateRedMaxMissilesRequired(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedMaxMissilesRequired = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedMissilesCurrentlyTargeting() const { return parameterHandleInitiateRedMissilesCurrentlyTargeting; }
void AdminFederateAmbassador::setParamInitiateRedMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedMissilesCurrentlyTargeting = handle; }

// Initiate Handshake - BLUE
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassInitiateBlueHandshake() const { return interactionClassInitiateBlueHandshake; }
void AdminFederateAmbassador::setInteractionClassInitiateBlueHandshake(const rti1516e::InteractionClassHandle& handle) { interactionClassInitiateBlueHandshake = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueShooterID() const { return parameterHandleInitiateBlueShooterID; }
void AdminFederateAmbassador::setParamInitiateBlueShooterID(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueShooterID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueMissilesAvailable() const { return parameterHandleInitiateBlueMissilesAvailable; }
void AdminFederateAmbassador::setParamInitiateBlueMissilesAvailable(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueMissilesAvailable = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueTargetID() const { return parameterHandleInitiateBlueTargetID; }
void AdminFederateAmbassador::setParamInitiateBlueTargetID(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueTargetID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueMaxMissilesRequired() const { return parameterHandleInitiateBlueMaxMissilesRequired; }
void AdminFederateAmbassador::setParamInitiateBlueMaxMissilesRequired(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueMaxMissilesRequired = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueMissilesCurrentlyTargeting() const { return parameterHandleInitiateBlueMissilesCurrentlyTargeting; }
void AdminFederateAmbassador::setParamInitiateBlueMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueMissilesCurrentlyTargeting = handle; }

// CONFIRM HANDSHAKE - RED
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassConfirmRedHandshake() const { return interactionClassConfirmRedHandshake; }
void AdminFederateAmbassador::setInteractionClassConfirmRedHandshake(const rti1516e::InteractionClassHandle& handle) { interactionClassConfirmRedHandshake = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedShooterID() const { return parameterHandleConfirmRedShooterID; }
void AdminFederateAmbassador::setParamConfirmRedShooterID(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmRedShooterID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedMissilesLoaded() const { return parameterHandleConfirmRedMissilesLoaded; }
void AdminFederateAmbassador::setParamConfirmRedMissilesLoaded(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmRedMissilesLoaded = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedTargetID() const { return parameterHandleConfirmRedTargetID; }
void AdminFederateAmbassador::setParamConfirmRedTargetID(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmRedTargetID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedCurrentlyTargeting() const { return parameterHandleConfirmRedCurrentlyTargeting; }
void AdminFederateAmbassador::setParamConfirmRedCurrentlyTargeting(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmRedCurrentlyTargeting = handle; }

// CONFIRM HANDSHAKE - BLUE
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassConfirmBlueHandshake() const { return interactionClassConfirmBlueHandshake; }
void AdminFederateAmbassador::setInteractionClassConfirmBlueHandshake(const rti1516e::InteractionClassHandle& handle) { interactionClassConfirmBlueHandshake = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueShooterID() const { return parameterHandleConfirmBlueShooterID; }
void AdminFederateAmbassador::setParamConfirmBlueShooterID(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmBlueShooterID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueMissilesLoaded() const { return parameterHandleConfirmBlueMissilesLoaded; }
void AdminFederateAmbassador::setParamConfirmBlueMissilesLoaded(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmBlueMissilesLoaded = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueTargetID() const { return parameterHandleConfirmBlueTargetID; }
void AdminFederateAmbassador::setParamConfirmBlueTargetID(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmBlueTargetID = handle; }

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueCurrentlyTargeting() const { return parameterHandleConfirmBlueCurrentlyTargeting; }
void AdminFederateAmbassador::setParamConfirmBlueCurrentlyTargeting(const rti1516e::ParameterHandle& handle) { parameterHandleConfirmBlueCurrentlyTargeting = handle; }