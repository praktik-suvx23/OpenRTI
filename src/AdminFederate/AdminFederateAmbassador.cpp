#include "AdminFederateAmbassador.h"

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
    rti1516e::SupplementalReceiveInfo receiveInfo)
{
    Team team = Team::UNSIGNED;
    rti1516e::ParameterHandle shooterIDHandle;
    rti1516e::ParameterHandle missilesAvailableHandle;
    rti1516e::ParameterHandle targetIDHandle;
    rti1516e::ParameterHandle maxMissilesRequiredHandle;
    rti1516e::ParameterHandle missilesCurrentlyTargetingHandle;
    rti1516e::ParameterHandle distanceToTargetHandle;

    if (interactionClassHandle == interactionClassInitiateBlueHandshake) {
        team = Team::Blue;
        shooterIDHandle = parameterHandleInitiateBlueShooterID;
        missilesAvailableHandle = parameterHandleInitiateBlueMissilesAvailable;
        targetIDHandle = parameterHandleInitiateBlueTargetID;
        maxMissilesRequiredHandle = parameterHandleInitiateBlueMaxMissilesRequired;
        missilesCurrentlyTargetingHandle = parameterHandleInitiateBlueMissilesCurrentlyTargeting;
        distanceToTargetHandle = parameterHandleInitiateBlueDistanceToTarget;

    } else if (interactionClassHandle == interactionClassInitiateRedHandshake) {
        team = Team::Red;
        shooterIDHandle = parameterHandleInitiateRedShooterID;
        missilesAvailableHandle = parameterHandleInitiateRedMissilesAvailable;
        targetIDHandle = parameterHandleInitiateRedTargetID;
        maxMissilesRequiredHandle = parameterHandleInitiateRedMaxMissilesRequired;
        missilesCurrentlyTargetingHandle = parameterHandleInitiateRedMissilesCurrentlyTargeting;
        distanceToTargetHandle = parameterHandleInitiateRedDistanceToTarget;
    } else {
        return;
    }

    if (parameterValues.find(shooterIDHandle) != parameterValues.end() &&
        parameterValues.find(missilesAvailableHandle) != parameterValues.end() &&
        parameterValues.find(targetIDHandle) != parameterValues.end() &&
        parameterValues.find(maxMissilesRequiredHandle) != parameterValues.end() &&
        parameterValues.find(missilesCurrentlyTargetingHandle) != parameterValues.end() &&
        parameterValues.find(distanceToTargetHandle) != parameterValues.end())
    {
        rti1516e::HLAunicodeString hlaStr;
        rti1516e::HLAinteger32BE hlaInt;
        rti1516e::HLAfloat64BE hlaFloat;

        hlaStr.decode(parameterValues.at(shooterIDHandle));
        std::wstring shooterID = hlaStr.get();

        hlaInt.decode(parameterValues.at(missilesAvailableHandle));
        int32_t missilesAvailable = hlaInt.get();

        hlaStr.decode(parameterValues.at(targetIDHandle));
        std::wstring targetID = hlaStr.get();

        hlaInt.decode(parameterValues.at(maxMissilesRequiredHandle));
        int32_t maxMissilesRequired = hlaInt.get();

        hlaInt.decode(parameterValues.at(missilesCurrentlyTargetingHandle));
        int32_t missilesCurrentlyTargeting = hlaInt.get();

        hlaFloat.decode(parameterValues.at(distanceToTargetHandle));
        double distanceToTarget = hlaFloat.get();

        if (maxMissilesRequired > missilesCurrentlyTargeting) {
            InitialHandshake hs = { shooterID, missilesAvailable, targetID, maxMissilesRequired, missilesCurrentlyTargeting, distanceToTarget };

            if (team == Team::Blue)
                blueInitialHandshake.push_back(hs);
            else if (team == Team::Red)
                redInitialHandshake.push_back(hs);
        }
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
std::vector<InitialHandshake>& AdminFederateAmbassador::getInitialHandshakeBlue() {
    return blueInitialHandshake;
}

std::vector<InitialHandshake>& AdminFederateAmbassador::getInitialHandshakeRed() {
    return redInitialHandshake;
}

// missilesLeftToTarget methods
std::unordered_map<std::wstring, std::optional<int32_t>>& AdminFederateAmbassador::getMissilesLeftToTargetBlue() {
    return missilesLeftToTargetBlue;
}
std::unordered_map<std::wstring, std::optional<int32_t>>& AdminFederateAmbassador::getMissilesLeftToTargetRed() {
    return missilesLeftToTargetRed;
}

void AdminFederateAmbassador::setSyncLabel(std::wstring label) {
    syncLabel = label;}
void AdminFederateAmbassador::setSetupSimulationHandle(rti1516e::InteractionClassHandle handle) {
    setupSimulationHandle = handle;}
void AdminFederateAmbassador::setBlueShipsParam(rti1516e::ParameterHandle handle) {
    blueShipsParam = handle;}
void AdminFederateAmbassador::setRedShipsParam(rti1516e::ParameterHandle handle) {
    redShipsParam = handle;}
void AdminFederateAmbassador::setTimeScaleFactorParam(rti1516e::ParameterHandle handle) {
    timeScaleFactorParam = handle;}

std::wstring AdminFederateAmbassador::getSyncLabel() {
    return syncLabel;}
rti1516e::InteractionClassHandle AdminFederateAmbassador::getSetupSimulationHandle() {
    return setupSimulationHandle;}
rti1516e::ParameterHandle AdminFederateAmbassador::getBlueShipsParam() {
    return blueShipsParam;}
rti1516e::ParameterHandle AdminFederateAmbassador::getRedShipsParam() {
    return redShipsParam;}
rti1516e::ParameterHandle AdminFederateAmbassador::getTimeScaleFactorParam() {
    return timeScaleFactorParam;}