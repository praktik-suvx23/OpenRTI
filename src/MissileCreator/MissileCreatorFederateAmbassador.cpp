#include "MissileCreatorFederateAmbassador.h"


MissileCreatorFederateAmbassador::MissileCreatorFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {
}
MissileCreatorFederateAmbassador::~MissileCreatorFederateAmbassador() {
}

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
void MissileCreatorFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const& theObjectName) {
    //std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
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
        //Somehow know where to start missileFederates based on simulation load.
        //Send start missile interaction to the missile federate

        sendStartMissileInteraction( //Replace with actual values
            L"shooterID", 
            L"Blue", 
            std::make_pair(0.0, 0.0), 
            std::make_pair(1.0, 1.0), 
            10000.0, 
            300.0,
            1, 
            45.0 
        );

    }
}

void MissileCreatorFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const& theAttributes,
    rti1516e::VariableLengthData const& theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    // Handle attribute values
}


void MissileCreatorFederateAmbassador::sendStartMissileInteraction( 
    std::wstring shooterID,
    std::wstring missileTeam,
    std::pair<double, double> missileStartPosition,
    std::pair<double, double> missileTargetPosition,
    double missileAltitude,
    double missileSpeed,
    int numberOfMissilesFired,
    double initialBearing) {
    // Send the start missile interaction to the missile federate
    // This function should be implemented to send the interaction with the required parameters
    // using the RTIambassador.
    std::wcout << L"[INFO] Sending StartMissile interaction." << std::endl;

    rti1516e::ParameterHandleValueMap parameters;
    parameters[parameterHandleCreateMissileID] = rti1516e::HLAunicodeString(shooterID).encode();
    parameters[parameterHandleCreateMissileTeam] = rti1516e::HLAunicodeString(missileTeam).encode();
    
    rti1516e::HLAfixedRecord startPositionRecord;
    startPositionRecord.appendElement(rti1516e::HLAfloat64BE(missileStartPosition.first));
    startPositionRecord.appendElement(rti1516e::HLAfloat64BE(missileStartPosition.second));
    parameters[parameterHandleCreateMissilePosition] = startPositionRecord.encode();

    rti1516e::HLAfixedRecord targetPositionRecord;
    targetPositionRecord.appendElement(rti1516e::HLAfloat64BE(missileTargetPosition.first));
    targetPositionRecord.appendElement(rti1516e::HLAfloat64BE(missileTargetPosition.second));
    parameters[parameterHandleCreateMissileTargetPosition] = targetPositionRecord.encode();

    parameters[parameterHandleCreateMissileAltitude] = rti1516e::HLAfloat64BE(missileAltitude).encode();

    parameters[parameterHandleCreateMissileNumberOfMissilesFired] = rti1516e::HLAinteger32BE(numberOfMissilesFired).encode();
    parameters[parameterHandleCreateMissileBearing] = rti1516e::HLAfloat64BE(initialBearing).encode();
    try {

        _rtiAmbassador->sendInteraction( //receiver needs to be able to handle receive interaction without time
            interactionClassCreateMissile,
            parameters,
            rti1516e::VariableLengthData()
        );

        std::wcout << L"[INFO] StartMissile interaction sent." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Failed to send StartMissile interaction: " << e.what() << std::endl;
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

//get set create missile interaction
void MissileCreatorFederateAmbassador::setInteractioClassCreateMissile(rti1516e::InteractionClassHandle interactionClassHandle) {
    interactionClassCreateMissile = interactionClassHandle;
}
rti1516e::InteractionClassHandle MissileCreatorFederateAmbassador::getInteractioClassCreateMissile() const {
    return interactionClassCreateMissile;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileID(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileID = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileID() const {
    return parameterHandleCreateMissileID;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileTeam(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileTeam = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileTeam() const {
    return parameterHandleCreateMissileTeam;
}

void MissileCreatorFederateAmbassador::setParamCreateMissilePosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissilePosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissilePosition() const {
    return parameterHandleCreateMissilePosition;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileTargetPosition(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileTargetPosition = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileTargetPosition() const {
    return parameterHandleCreateMissileTargetPosition;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileAltitude(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileAltitude = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileAltitude() const {
    return parameterHandleCreateMissileAltitude;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileNumberOfMissilesFired = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileNumberOfMissilesFired() const {
    return parameterHandleCreateMissileNumberOfMissilesFired;
}

void MissileCreatorFederateAmbassador::setParamCreateMissileBearing(rti1516e::ParameterHandle parameterHandle) {
    parameterHandleCreateMissileBearing = parameterHandle;
}
rti1516e::ParameterHandle MissileCreatorFederateAmbassador::getParamCreateMissileBearing() const {
    return parameterHandleCreateMissileBearing;
}
//------
//--- End of get set methods ---
