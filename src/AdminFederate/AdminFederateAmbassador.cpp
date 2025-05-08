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

// ===== INITIATE RED =====
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassInitiateRedHandshake() const {
    return interactionClassInitiateRedHandshake;
}
void AdminFederateAmbassador::setInteractionClassInitiateRedHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassInitiateRedHandshake = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedShooterID() const {
    return parameterHandleInitiateRedShooterID;
}
void AdminFederateAmbassador::setParamInitiateRedShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateRedShooterID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedShooterTeam() const {
    return parameterHandleInitiateRedShooterTeam;
}
void AdminFederateAmbassador::setParamInitiateRedShooterTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateRedShooterTeam = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedTargetID() const {
    return parameterHandleInitiateRedTargetID;
}
void AdminFederateAmbassador::setParamInitiateRedTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateRedTargetID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedMissileAmountFired() const {
    return parameterHandleInitiateRedMissileAmountFired;
}
void AdminFederateAmbassador::setParamInitiateRedMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateRedMissileAmountFired = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedOrderID() const {
    return parameterHandleInitiateRedOrderID;
}
void AdminFederateAmbassador::setParamInitiateRedOrderID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateRedOrderID = handle;
}


// ===== INITIATE BLUE =====
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassInitiateBlueHandshake() const {
    return interactionClassInitiateBlueHandshake;
}
void AdminFederateAmbassador::setInteractionClassInitiateBlueHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassInitiateBlueHandshake = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueShooterID() const {
    return parameterHandleInitiateBlueShooterID;
}
void AdminFederateAmbassador::setParamInitiateBlueShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateBlueShooterID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueShooterTeam() const {
    return parameterHandleInitiateBlueShooterTeam;
}
void AdminFederateAmbassador::setParamInitiateBlueShooterTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateBlueShooterTeam = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueTargetID() const {
    return parameterHandleInitiateBlueTargetID;
}
void AdminFederateAmbassador::setParamInitiateBlueTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateBlueTargetID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueMissileAmountFired() const {
    return parameterHandleInitiateBlueMissileAmountFired;
}
void AdminFederateAmbassador::setParamInitiateBlueMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateBlueMissileAmountFired = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueOrderID() const {
    return parameterHandleInitiateBlueOrderID;
}
void AdminFederateAmbassador::setParamInitiateBlueOrderID(const rti1516e::ParameterHandle& handle) {
    parameterHandleInitiateBlueOrderID = handle;
}


// ===== CONFIRM RED =====
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassConfirmRedHandshake() const {
    return interactionClassConfirmRedHandshake;
}
void AdminFederateAmbassador::setInteractionClassConfirmRedHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassConfirmRedHandshake = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedShooterID() const {
    return parameterHandleConfirmRedShooterID;
}
void AdminFederateAmbassador::setParamConfirmRedShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmRedShooterID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedShooterTeam() const {
    return parameterHandleConfirmRedShooterTeam;
}
void AdminFederateAmbassador::setParamConfirmRedShooterTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmRedShooterTeam = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedTargetID() const {
    return parameterHandleConfirmRedTargetID;
}
void AdminFederateAmbassador::setParamConfirmRedTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmRedTargetID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedMissileAmountFired() const {
    return parameterHandleConfirmRedMissileAmountFired;
}
void AdminFederateAmbassador::setParamConfirmRedMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmRedMissileAmountFired = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmRedOrderID() const {
    return parameterHandleConfirmRedOrderID;
}
void AdminFederateAmbassador::setParamConfirmRedOrderID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmRedOrderID = handle;
}


// ===== CONFIRM BLUE =====
rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassConfirmBlueHandshake() const {
    return interactionClassConfirmBlueHandshake;
}
void AdminFederateAmbassador::setInteractionClassConfirmBlueHandshake(const rti1516e::InteractionClassHandle& handle) {
    interactionClassConfirmBlueHandshake = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueShooterID() const {
    return parameterHandleConfirmBlueShooterID;
}
void AdminFederateAmbassador::setParamConfirmBlueShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmBlueShooterID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueShooterTeam() const {
    return parameterHandleConfirmBlueShooterTeam;
}
void AdminFederateAmbassador::setParamConfirmBlueShooterTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmBlueShooterTeam = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueTargetID() const {
    return parameterHandleConfirmBlueTargetID;
}
void AdminFederateAmbassador::setParamConfirmBlueTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmBlueTargetID = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueMissileAmountFired() const {
    return parameterHandleConfirmBlueMissileAmountFired;
}
void AdminFederateAmbassador::setParamConfirmBlueMissileAmountFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmBlueMissileAmountFired = handle;
}
rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBlueOrderID() const {
    return parameterHandleConfirmBlueOrderID;
}
void AdminFederateAmbassador::setParamConfirmBlueOrderID(const rti1516e::ParameterHandle& handle) {
    parameterHandleConfirmBlueOrderID = handle;
}
