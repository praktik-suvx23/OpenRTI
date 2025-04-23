#include "MissileCreatorFederateAmbassador.h"


void MissileCreatorFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"InitialSync") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
}

void MissileCreatorFederateAmbassador::receiveInteraction(    //Receive interaction without time
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Received interaction: " << interactionClassHandle << std::endl;

    if (interactionClassHandle == interactionClassFireMissile) {
        // Handle the interaction
        std::wcout << L"[INFO] FireMissile interaction received." << std::endl;
        // Process the parameters as needed
    }
}


//--- Get set methods Below ---
std::wstring MissileCreatorFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

// Get set fire missile interaction
void MissileCreatorFederateAmbassador::setInteractioClassFireMissile(rti1516e::InteractionClassHandle interactionClassHandle) {
    interactionClassFireMissile = interactionClassHandle;
}
rti1516e::InteractionClassHandle MissileCreatorFederateAmbassador::getInteractioClassFireMissile() const {
    return interactionClassFireMissile;
}

void MissileCreatorFederateAmbassador::setParamShooterID(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleShooterID = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}

void MissileCreatorFederateAmbassador::setParamMissileTeam(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileTeam = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}

void MissileCreatorFederateAmbassador::setParamMissileStartPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileStartPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}

void MissileCreatorFederateAmbassador::setParamMissileTargetPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileTargetPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}

void MissileCreatorFederateAmbassador::setParamNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleNumberOfMissilesFired = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MissileCreatorFederateAmbassador::setParamMissileSpeed(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleMissileSpeed = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
//------
