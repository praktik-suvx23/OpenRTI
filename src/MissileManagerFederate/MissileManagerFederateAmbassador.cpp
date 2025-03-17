#include "MissileManagerFederateAmbassador.h"
#include "../include/decodePosition.h"

MissileManagerAmbassador::MissileManagerAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MissileManagerAmbassador::~MissileManagerAmbassador() {}

void MissileManagerAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"Synchronization point announced: " << label << std::endl;
}

void MissileManagerAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void MissileManagerAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
    std::wstring tempShipID;

    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        
        if (attributeValueFederateName.get() != TargetFederate) {
            return;
        } else {
            std::wcout << L"Instance " << instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
        }
        if (_shipInstances.find(theObject) != _shipInstances.end()) {
            auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
            auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
            auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
            auto itShipSize = theAttributes.find(attributeHandleShipSize);
            auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfRobots);
    
            if (itShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipPosition;
                attributeValueShipPosition.decode(itShipPosition->second);
                shipPosition = attributeValueShipPosition.get();
            }
            if (itFutureShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueFutureShipPosition;
                //attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
                //expectedShipPosition = attributeValueFutureShipPosition.get();
            }
            if (itShipSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSpeed;
                //attributeValueShipSpeed.decode(itShipSpeed->second);
            }
            if (itShipSize != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSize;
                attributeValueShipSize.decode(itShipSize->second);
                shipSize = attributeValueShipSize.get();
                
            }
            if (itNumberOfRobots != theAttributes.end()) {
                rti1516e::HLAinteger32BE attributeValueNumberOfRobots;
                attributeValueNumberOfRobots.decode(itNumberOfRobots->second);
                setNumberOfRobots(attributeValueNumberOfRobots.get());
            }
        }
    }
}

void MissileManagerAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    std::wcout << L"[DEBUG] receivedInteraction: " << interactionClassHandle << std::endl;
    
    if (fireMissileHandle == interactionClassHandle) {
        std::wcout << L"[INFO] FireMissile interaction received" << std::endl;

        bool hasShooterID = false, hasTargetID = false, hasShooterPosition = false, hasTargetPosition = false;

        for (const auto& param : parameterValues) {
            if (param.first == shooterIDParamHandle) {
                rti1516e::HLAunicodeString hlaShooterID;
                hlaShooterID.decode(param.second);
                setShooterIDParamHandle(*this, hlaShooterID.get());
                hasShooterID = true;
            }
            else if (param.first == targetIDParamHandle) {
                rti1516e::HLAunicodeString hlaTargetID;
                hlaTargetID.decode(param.second);
                setTargetIDParamHandle(*this, hlaTargetID.get());
                hasTargetID = true;
            }
            else if (param.first == shooterPositionParamHandle) {
                std::pair<double, double> position = decodePositionRec(param.second);
                setShooterPositionParamHandle(*this, position);
                hasShooterPosition = true;
            }
            else if (param.first == targetPositionParamHandle) {
                std::pair<double, double> position = decodePositionRec(param.second);
                setTargetPositionParamHandle(*this, position);
                hasTargetPosition = true;
            }
            /*
            TODO: Add the rest of the parameters
                missileCountParamHandle     Based on ship size, fire X number of missiles
                missileTypeParamHandle      Different missile types
                maxDistanceParamHandle      Missile max travel distance
                missileSpeedParamHandle     Missile speed
                lockOnDistanceParamHandle   Distance to missile to auto lock on target
                fireTimeParamHandle         Simulation time, might not be nessessary / get from somewhere else
            */
        }

        if (!hasShooterID || !hasTargetID || !hasShooterPosition || !hasTargetPosition) {
            std::wcerr << L"[ERROR] Missing required missile data!" << std::endl;
            return;
        }

        setFlagActiveMissile(*this, true);
        // Flag that valid missile data was received
        std::wcout << L"[INFO] Valid missile data received." << std::endl;
    }
}

void MissileManagerAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MissileManagerAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MissileManagerAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) { //Used for time management
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

//getters and setters for attributes
double MissileManagerAmbassador::getCurrentAltitude() const {
    return currentAltitude;
}
void MissileManagerAmbassador::setCurrentAltitude(double altitude) {
    currentAltitude = altitude;
}

double MissileManagerAmbassador::getCurrentSpeed() const {
    return currentSpeed;
}
void MissileManagerAmbassador::setCurrentSpeed(const double& speed) {
    currentSpeed = speed;
}

double MissileManagerAmbassador::getCurrentFuelLevel() const {
    return currentFuelLevel;
}
void MissileManagerAmbassador::setCurrentFuelLevel(const double& fuelLevel) {
    currentFuelLevel = fuelLevel;
}

std::wstring MissileManagerAmbassador::getCurrentPosition() const {
    return currentPosition;
}
void MissileManagerAmbassador::setCurrentPosition(const std::wstring& position) {
    currentPosition = position;
}

double MissileManagerAmbassador::getCurrentDistance() const {
    return currentDistance;
}
void MissileManagerAmbassador::setCurrentDistance(const double& distance) {
    currentDistance = distance;
}
int MissileManagerAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MissileManagerAmbassador::setNumberOfRobots(const int& robots) {
    numberOfRobots = robots;
}

bool MissileManagerAmbassador::getFlagActiveMissile() const {
    return flagActiveMissile;
}
void MissileManagerAmbassador::setFlagActiveMissile(bool flag) {
    flagActiveMissile = flag;
}

std::wstring MissileManagerAmbassador::getShooterID() const {
    return shooterID;
}
void MissileManagerAmbassador::setShooterID(const std::wstring& id) {
    shooterID = id;
}

std::wstring MissileManagerAmbassador::getTargetID() const {
    return targetID;
}
void MissileManagerAmbassador::setTargetID(const std::wstring& id) {
    targetID = id;
}

std::wstring MissileManagerAmbassador::getMissileType() const {
    return missileType;
}
void MissileManagerAmbassador::setMissileType(const std::wstring& type) {
    missileType = type;
}

double MissileManagerAmbassador::getMissileSpeed() const {
    return missileSpeed;
}
void MissileManagerAmbassador::setMissileSpeed(double speed) {
    missileSpeed = speed;
}

double MissileManagerAmbassador::getMaxDistance() const {
    return maxDistance;
}
void MissileManagerAmbassador::setMaxDistance(double distance) {
    maxDistance = distance;
}

double MissileManagerAmbassador::getLockOnDistance() const {
    return lockOnDistance;
}
void MissileManagerAmbassador::setLockOnDistance(double distance) {
    lockOnDistance = distance;
}

double MissileManagerAmbassador::getFireTime() const {
    return fireTime;
}
void MissileManagerAmbassador::setFireTime(double time) {
    fireTime = time;
}

int MissileManagerAmbassador::getMissileCount() const {
    return missileCount;
}
void MissileManagerAmbassador::setMissileCount(int count) {
    missileCount = count;
}

std::pair<double, double> MissileManagerAmbassador::getShooterPosition() const {
    return shooterPosition;
}
void MissileManagerAmbassador::setShooterPosition(const std::pair<double, double>& position) {
    shooterPosition = position;
}

std::pair<double, double> MissileManagerAmbassador::getTargetPosition() const {
    return targetPosition;
}
void MissileManagerAmbassador::setTargetPosition(const std::pair<double, double>& position) {
    targetPosition = position;
}

// general get and set functions
std::wstring MissileManagerAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MissileManagerAmbassador::getFederateName() const {
    return federateName;
}
void MissileManagerAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

std::wstring MissileManagerAmbassador::getExpectedShipName() const {
    return _expectedShipName;
}
void MissileManagerAmbassador::setExpectedShipName(const std::wstring& name) {
    _expectedShipName = name;
}

std::wstring MissileManagerAmbassador::getTargetFederate() const {
    return TargetFederate;
}
void MissileManagerAmbassador::setTargetFederate(const std::wstring& federate) {
    TargetFederate = federate;
}

std::wstring MissileManagerAmbassador::getRobotPosition() const {
    return RobotPosition;
}
void MissileManagerAmbassador::setRobotPosition(const std::wstring& position) {
    RobotPosition = position;
}

std::wstring MissileManagerAmbassador::getExpectedFuturePosition() const {
    return expectedFuturePosition;
}
void MissileManagerAmbassador::setExpectedFuturePosition(const std::wstring& position) {
    expectedFuturePosition = position;
}

std::vector<ClassActiveMissile> MissileManagerAmbassador::getActiveMissiles() const {
    return activeMissiles;
}
void MissileManagerAmbassador::setActiveMissiles(const std::vector<ClassActiveMissile>& missiles) {
    activeMissiles = missiles;
}

//get and set for fire interaction
rti1516e::InteractionClassHandle MissileManagerAmbassador::getFireRobotHandle() const {return fireRobotHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getFireRobotHandleParam() const {return fireParamHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getTargetParam() const {return targetParamHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getStartPosRobot() const {return startPosRobot;}
void MissileManagerAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {fireRobotHandle = handle;}
void MissileManagerAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {fireParamHandle = handle;}
void MissileManagerAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {targetParamHandle = handle;}
void MissileManagerAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {startPosRobot = handle;}

// Getters and setters for interaction class and parameter handles
rti1516e::InteractionClassHandle MissileManagerAmbassador::getFireMissileHandle() const {
    return fireMissileHandle;
}
void MissileManagerAmbassador::setFireMissileHandle(const rti1516e::InteractionClassHandle& handle) {
    fireMissileHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getShooterIDParamHandle() const {
    return shooterIDParamHandle;
}
void MissileManagerAmbassador::setShooterIDParamHandle(const rti1516e::ParameterHandle& handle) {
    shooterIDParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getTargetIDParamHandle() const {
    return targetIDParamHandle;
}
void MissileManagerAmbassador::setTargetIDParamHandle(const rti1516e::ParameterHandle& handle) {
    targetIDParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getShooterPositionParamHandle() const {
    return shooterPositionParamHandle;
}
void MissileManagerAmbassador::setShooterPositionParamHandle(const rti1516e::ParameterHandle& handle) {
    shooterPositionParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getTargetPositionParamHandle() const {
    return targetPositionParamHandle;
}
void MissileManagerAmbassador::setTargetPositionParamHandle(const rti1516e::ParameterHandle& handle) {
    targetPositionParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getMissileCountParamHandle() const {
    return missileCountParamHandle;
}
void MissileManagerAmbassador::setMissileCountParamHandle(const rti1516e::ParameterHandle& handle) {
    missileCountParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getMissileTypeParamHandle() const {
    return missileTypeParamHandle;
}
void MissileManagerAmbassador::setMissileTypeParamHandle(const rti1516e::ParameterHandle& handle) {
    missileTypeParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getMaxDistanceParamHandle() const {
    return maxDistanceParamHandle;
}
void MissileManagerAmbassador::setMaxDistanceParamHandle(const rti1516e::ParameterHandle& handle) {
    maxDistanceParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getMissileSpeedParamHandle() const {
    return missileSpeedParamHandle;
}
void MissileManagerAmbassador::setMissileSpeedParamHandle(const rti1516e::ParameterHandle& handle) {
    missileSpeedParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getLockOnDistanceParamHandle() const {
    return lockOnDistanceParamHandle;
}
void MissileManagerAmbassador::setLockOnDistanceParamHandle(const rti1516e::ParameterHandle& handle) {
    lockOnDistanceParamHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getFireTimeParamHandle() const {
    return fireTimeParamHandle;
}
void MissileManagerAmbassador::setFireTimeParamHandle(const rti1516e::ParameterHandle& handle) {
    fireTimeParamHandle = handle;
}

// Getters and setters for object class and attribute handles
rti1516e::ObjectClassHandle MissileManagerAmbassador::getShipClassHandle() const {
    return shipClassHandle;
}
void MissileManagerAmbassador::setShipClassHandle(const rti1516e::ObjectClassHandle& handle) {
    shipClassHandle = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipID() const {
    return attributeHandleShipID;
}
void MissileManagerAmbassador::setAttributeHandleShipID(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipID = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void MissileManagerAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MissileManagerAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MissileManagerAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSize() const {
    return attributeHandleShipSize;
}
void MissileManagerAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSize = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipAngle() const {
    return attributeHandleShipAngle;
}
void MissileManagerAmbassador::setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipAngle = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void MissileManagerAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
}

rti1516e::ObjectClassHandle MissileManagerAmbassador::getMissileClassHandle() const {
    return missileClassHandle;
}
void MissileManagerAmbassador::setMissileClassHandle(const rti1516e::ObjectClassHandle& handle) {
    missileClassHandle = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleMissileID() const {
    return attributeHandleMissileID;
}
void MissileManagerAmbassador::setAttributeHandleMissileID(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileID = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleSpeed() const {
    return attributeHandleSpeed;
}
void MissileManagerAmbassador::setAttributeHandleSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleSpeed = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleFuelLevel() const {
    return attributeHandleFuelLevel;
}
void MissileManagerAmbassador::setAttributeHandleFuelLevel(const rti1516e::AttributeHandle& handle) {
    attributeHandleFuelLevel = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandlePosition() const {
    return attributeHandlePosition;
}
void MissileManagerAmbassador::setAttributeHandlePosition(const rti1516e::AttributeHandle& handle) {
    attributeHandlePosition = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeAltitude() const {
    return attributeAltitude;
}
void MissileManagerAmbassador::setAttributeAltitude(const rti1516e::AttributeHandle& handle) {
    attributeAltitude = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleTarget() const {
    return attributeHandleTarget;
}
void MissileManagerAmbassador::setAttributeHandleTarget(const rti1516e::AttributeHandle& handle) {
    attributeHandleTarget = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleMissileTeam() const {
    return attributeHandleMissileTeam;
}
void MissileManagerAmbassador::setAttributeHandleMissileTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileTeam = handle;
}