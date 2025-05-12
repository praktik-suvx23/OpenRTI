#include "AdminFederateAmbassador.h"
#include "AdminFederateAmbassadorHelper.h"

AdminFederateAmbassador::AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {}

AdminFederateAmbassador::~AdminFederateAmbassador() {}

void AdminFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {

    if (interactionClassHandle == interactionClassInitiateBlueHandshake) {
        auto itParamShooterID = parameterValues.find(parameterHandleInitiateBlueShooterID);
        auto itParamMissilesAvailable = parameterValues.find(parameterHandleInitiateBlueMissilesAvailable);
        auto itParamTargetID = parameterValues.find(parameterHandleInitiateBlueTargetID);
        auto itParamMaxMissilesRequired = parameterValues.find(parameterHandleInitiateBlueMaxMissilesRequired);
        auto itParamMissilesCurrentlyTargeting = parameterValues.find(parameterHandleInitiateBlueMissilesCurrentlyTargeting);
        auto itParamDistanceToTarget = parameterValues.find(parameterHandleInitiateBlueDistanceToTarget);

        if (itParamShooterID != parameterValues.end() && itParamMissilesAvailable != parameterValues.end() &&
            itParamTargetID != parameterValues.end() && itParamMaxMissilesRequired != parameterValues.end() &&
            itParamMissilesCurrentlyTargeting != parameterValues.end() && itParamDistanceToTarget != parameterValues.end()) {
            rti1516e::HLAunicodeString tempString;
            rti1516e::HLAinteger32BE tempInt;
            rti1516e::HLAfloat64BE tempFloat;
            tempString.decode(itParamShooterID->second);
            std::wstring shooterID = tempString.get();
            tempInt.decode(itParamMissilesAvailable->second);
            int32_t missilesAvailable = tempInt.get();
            tempString.decode(itParamTargetID->second);
            std::wstring targetID = tempString.get();
            tempInt.decode(itParamMaxMissilesRequired->second);
            int32_t maxMissilesRequired = tempInt.get();
            tempInt.decode(itParamMissilesCurrentlyTargeting->second);
            int32_t missilesCurrentlyTargeting = tempInt.get();
            tempFloat.decode(itParamDistanceToTarget->second);
            double distanceToTarget = tempFloat.get();

            if (maxMissilesRequired >= missilesCurrentlyTargeting) {
                InitialHandshake targetInfo = InitialHandshake{shooterID, missilesAvailable, targetID, maxMissilesRequired, missilesCurrentlyTargeting, distanceToTarget};
                blueInitialHandshakeMap[targetInfo] = true;
            }
            
        }
    } else if (interactionClassHandle == interactionClassInitiateRedHandshake) {
        auto itParamShooterID = parameterValues.find(parameterHandleInitiateRedShooterID);
        auto itParamMissilesAvailable = parameterValues.find(parameterHandleInitiateRedMissilesAvailable);
        auto itParamTargetID = parameterValues.find(parameterHandleInitiateRedTargetID);
        auto itParamMaxMissilesRequired = parameterValues.find(parameterHandleInitiateRedMaxMissilesRequired);
        auto itParamMissilesCurrentlyTargeting = parameterValues.find(parameterHandleInitiateRedMissilesCurrentlyTargeting);
        auto itParamDistanceToTarget = parameterValues.find(parameterHandleInitiateRedDistanceToTarget);
    
        if (itParamShooterID != parameterValues.end() && itParamMissilesAvailable != parameterValues.end() &&
            itParamTargetID != parameterValues.end() && itParamMaxMissilesRequired != parameterValues.end() &&
            itParamMissilesCurrentlyTargeting != parameterValues.end() && itParamDistanceToTarget != parameterValues.end()) {
            
            rti1516e::HLAunicodeString tempString;
            rti1516e::HLAinteger32BE tempInt;
            rti1516e::HLAfloat64BE tempFloat;
            
            tempString.decode(itParamShooterID->second);
            std::wstring shooterID = tempString.get();
            tempInt.decode(itParamMissilesAvailable->second);
            int32_t missilesAvailable = tempInt.get();
            tempString.decode(itParamTargetID->second);
            std::wstring targetID = tempString.get();
            tempInt.decode(itParamMaxMissilesRequired->second);
            int32_t maxMissilesRequired = tempInt.get();
            tempInt.decode(itParamMissilesCurrentlyTargeting->second);
            int32_t missilesCurrentlyTargeting = tempInt.get();
            tempFloat.decode(itParamDistanceToTarget->second);
            double distanceToTarget = tempInt.get();
    
            if (maxMissilesRequired >= missilesCurrentlyTargeting) {
                InitialHandshake targetInfo = InitialHandshake{shooterID, missilesAvailable, targetID, maxMissilesRequired, missilesCurrentlyTargeting, distanceToTarget};
                
                redInitialHandshakeMap[targetInfo] = true;
            }
        }
    } else {
        std::wcout << L"[ERROR] interactionClassInitiateBluehandshake = " << interactionClassInitiateBlueHandshake
                    << L" interactionClassInitiateRedHandshake = " << interactionClassInitiateRedHandshake
                    << L" interactionClassHandle = " << interactionClassHandle << std::endl;
    }
}

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

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateRedDistanceToTarget() const { return parameterHandleInitiateRedDistanceToTarget; }
void AdminFederateAmbassador::setParamInitiateRedDistanceToTarget(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateRedDistanceToTarget = handle; }

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

rti1516e::ParameterHandle AdminFederateAmbassador::getParamInitiateBlueDistanceToTarget() const { return parameterHandleInitiateBlueDistanceToTarget; }
void AdminFederateAmbassador::setParamInitiateBlueDistanceToTarget(const rti1516e::ParameterHandle& handle) { parameterHandleInitiateBlueDistanceToTarget = handle; }

rti1516e::InteractionClassHandle AdminFederateAmbassador::getInteractionClassConfirmHandshake(Team side) const {
    return (side == Team::Blue) ? interactionClassConfirmBlueHandshake
                                : interactionClassConfirmRedHandshake;
}

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmShooterID(Team side) const {
    return (side == Team::Blue) ? parameterHandleConfirmBlueShooterID
                                : parameterHandleConfirmRedShooterID;
}

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmMissilesLoaded(Team side) const {
    return (side == Team::Blue) ? parameterHandleConfirmBlueMissilesLoaded
                                : parameterHandleConfirmRedMissilesLoaded;
}

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmTargetID(Team side) const {
    return (side == Team::Blue) ? parameterHandleConfirmBlueTargetID
                                : parameterHandleConfirmRedTargetID;
}

rti1516e::ParameterHandle AdminFederateAmbassador::getParamConfirmBoolean(Team side) const {
    return (side == Team::Blue) ? parameterHandleConfirmBlueBoolean
                                : parameterHandleConfirmRedBoolean;
}

void AdminFederateAmbassador::setInteractionClassConfirmHandshake(Team side, const rti1516e::InteractionClassHandle& handle) {
    if (side == Team::Blue)
        interactionClassConfirmBlueHandshake = handle;
    else
        interactionClassConfirmRedHandshake = handle;
}

void AdminFederateAmbassador::setParamConfirmShooterID(Team side, const rti1516e::ParameterHandle& handle) {
    if (side == Team::Blue)
        parameterHandleConfirmBlueShooterID = handle;
    else
        parameterHandleConfirmRedShooterID = handle;
}

void AdminFederateAmbassador::setParamConfirmMissilesLoaded(Team side, const rti1516e::ParameterHandle& handle) {
    if (side == Team::Blue)
        parameterHandleConfirmBlueMissilesLoaded = handle;
    else
        parameterHandleConfirmRedMissilesLoaded = handle;
}

void AdminFederateAmbassador::setParamConfirmTargetID(Team side, const rti1516e::ParameterHandle& handle) {
    if (side == Team::Blue)
        parameterHandleConfirmBlueTargetID = handle;
    else
        parameterHandleConfirmRedTargetID = handle;
}

void AdminFederateAmbassador::setParamConfirmBoolean(Team side, const rti1516e::ParameterHandle& handle) {
    if (side == Team::Blue)
        parameterHandleConfirmBlueBoolean = handle;
    else
        parameterHandleConfirmRedBoolean = handle;
}


// Get, set and clear - initialHandshakeMap
std::unordered_map<InitialHandshake, bool>& AdminFederateAmbassador::getInitialHandshakeBlue() {
    return blueInitialHandshakeMap;
}

std::unordered_map<InitialHandshake, bool>& AdminFederateAmbassador::getInitialHandshakeRed() {
    return redInitialHandshakeMap;
}

// missilesLeftToTarget methods
std::unordered_map<std::wstring, std::optional<int32_t>>& AdminFederateAmbassador::getMissilesLeftToTargetBlue() {
    return missilesLeftToTargetBlue;
}
std::unordered_map<std::wstring, std::optional<int32_t>>& AdminFederateAmbassador::getMissilesLeftToTargetRed() {
    return missilesLeftToTargetRed;
}

