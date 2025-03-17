#include "ShootShipFederateAmbassador.h"

MyShootShipFederateAmbassador::MyShootShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance) 
    : _rtiambassador(rtiAmbassador), instance(instance) {
    _expectedShipName = L"EnemyShipFederate " + std::to_wstring(instance);
}

MyShootShipFederateAmbassador::~MyShootShipFederateAmbassador() {}

void MyShootShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void MyShootShipFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    std::wcout << L"[DEBUG] Reflect attribute values called" << std::endl;

    auto itShipFederateName = theAttributes.find(attributeHandleMyShipFederateName);
    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        if (attributeValueFederateName.get() != _expectedShipName) {
            std::wcout << L"Instance " << instance << L": Unexpected ship name: " << attributeValueFederateName.get() << std::endl;
            return;
        } else {
            std::wcout << L"Instance " << instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
        }
    }
    //Calculate distance between ships
    auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
    auto itEnemyShipFederateName = theAttributes.find(attributeHandleEnemyShipFederateName);

    if (itEnemyShipPosition != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueEnemyShipPosition;
        attributeValueEnemyShipPosition.decode(itEnemyShipPosition->second);
        setEnemyShipPosition(attributeValueEnemyShipPosition.get());
        std::wcout << L"Enemy ship position set: " << getEnemyShipPosition() << std::endl;
    }
    if (itEnemyShipFederateName != theAttributes.end()) { //Use this to start a robot to shoot at this federateName 
        rti1516e::HLAunicodeString attributeValueEnemyShipFederateName;
        attributeValueEnemyShipFederateName.decode(itEnemyShipFederateName->second);
        setEnemyShipFederateName(attributeValueEnemyShipFederateName.get());
        std::wcout << L"Enemy ship federate name set: " << getEnemyShipFederateName() << std::endl;
    }
}


void MyShootShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Recieve interaction called" << std::endl;
}

void MyShootShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"Shooter Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
}

void MyShootShipFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MyShootShipFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MyShootShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

//getter and setter for object class Ship and its attributes
rti1516e::ObjectClassHandle MyShootShipFederateAmbassador::getObjectClassHandleShip() const {return shipClassHandle;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipID() const {return attributeHandleShipID;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipTeam() const {return attributeHandleShipTeam;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipPosition() const {return attributeHandleShipPosition;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipSpeed() const {return attributeHandleShipSpeed;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipSize() const {return attributeHandleShipSize;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipAngle() const {return attributeHandleShipAngle;}
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {return attributeHandleNumberOfMissiles;}
void MyShootShipFederateAmbassador::setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle) {shipClassHandle = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipID(const rti1516e::AttributeHandle& handle) {attributeHandleShipID = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {attributeHandleShipTeam = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {attributeHandleShipPosition = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {attributeHandleShipSpeed = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {attributeHandleShipSize = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle) {attributeHandleShipAngle = handle;}
void MyShootShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {attributeHandleNumberOfMissiles = handle;}

// Getter and setter methods for FireMissile interaction class and parameters
rti1516e::InteractionClassHandle MyShootShipFederateAmbassador::getFireMissileHandle() const {return fireMissileHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getShooterIDParamHandle() const {return shooterIDParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTargetIDParamHandle() const {return targetIDParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getShooterPositionParamHandle() const {return shooterPositionParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTargetPositionParamHandle() const {return targetPositionParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getMissileCountParamHandle() const {return missileCountParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getMissileTypeParamHandle() const {return missileTypeParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getMaxDistanceParamHandle() const {return maxDistanceParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getMissileSpeedParamHandle() const {return missileSpeedParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getLockOnDistanceParamHandle() const {return lockOnDistanceParamHandle;}
rti1516e::ParameterHandle MyShootShipFederateAmbassador::getFireTimeParamHandle() const {return fireTimeParamHandle;}
void MyShootShipFederateAmbassador::setFireMissileHandle(rti1516e::InteractionClassHandle handle) {fireMissileHandle = handle;}
void MyShootShipFederateAmbassador::setShooterIDParamHandle(rti1516e::ParameterHandle handle) {shooterIDParamHandle = handle;}
void MyShootShipFederateAmbassador::setTargetIDParamHandle(rti1516e::ParameterHandle handle) {targetIDParamHandle = handle;}
void MyShootShipFederateAmbassador::setShooterPositionParamHandle(rti1516e::ParameterHandle handle) {shooterPositionParamHandle = handle;}
void MyShootShipFederateAmbassador::setTargetPositionParamHandle(rti1516e::ParameterHandle handle) {targetPositionParamHandle = handle;}
void MyShootShipFederateAmbassador::setMissileCountParamHandle(rti1516e::ParameterHandle handle) {missileCountParamHandle = handle;}
void MyShootShipFederateAmbassador::setMissileTypeParamHandle(rti1516e::ParameterHandle handle) {missileTypeParamHandle = handle;}
void MyShootShipFederateAmbassador::setMaxDistanceParamHandle(rti1516e::ParameterHandle handle) {maxDistanceParamHandle = handle;}
void MyShootShipFederateAmbassador::setMissileSpeedParamHandle(rti1516e::ParameterHandle handle) {missileSpeedParamHandle = handle;}
void MyShootShipFederateAmbassador::setLockOnDistanceParamHandle(rti1516e::ParameterHandle handle) {lockOnDistanceParamHandle = handle;}
void MyShootShipFederateAmbassador::setFireTimeParamHandle(rti1516e::ParameterHandle handle) {fireTimeParamHandle = handle;}

//Get and set for fire interaction
rti1516e::InteractionClassHandle MyShootShipFederateAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void MyShootShipFederateAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void MyShootShipFederateAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTargetParam() const {
    return TargetParam;
}
void MyShootShipFederateAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {
    TargetParam = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getStartPosRobot() const {
    return startPosRobot;
}
void MyShootShipFederateAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {
    startPosRobot = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTargetPositionParam() const {
    return targetPosition;
}
void MyShootShipFederateAmbassador::setTargetPositionParam(const rti1516e::ParameterHandle& handle) {
    targetPosition = handle;
}

// Getters and setters for ship attributes
std::wstring MyShootShipFederateAmbassador::getMyShipPosition() const {
    return myShipPosition;
}
void MyShootShipFederateAmbassador::setMyShipPosition(const std::wstring& position) {
    myShipPosition = position;
}

std::wstring MyShootShipFederateAmbassador::getMyShipFederateName() const {
    return myShipFederateName;
}
void MyShootShipFederateAmbassador::setMyShipFederateName(const std::wstring& name) {
    myShipFederateName = name;
}

double MyShootShipFederateAmbassador::getMyShipSpeed() const {
    return myShipSpeed;
}
void MyShootShipFederateAmbassador::setMyShipSpeed(const double& speed) {
    myShipSpeed = speed;
}

std::wstring MyShootShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void MyShootShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::wstring MyShootShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void MyShootShipFederateAmbassador::setEnemyShipPosition(const std::wstring& position) {
    enemyShipPosition = position;
}

double MyShootShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void MyShootShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double MyShootShipFederateAmbassador::getBearing() const {
    return bearing;
}
void MyShootShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool MyShootShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void MyShootShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring MyShootShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

//Json values get/set
std::wstring MyShootShipFederateAmbassador::getshipNumber() const {
    return shipNumber;
}
void MyShootShipFederateAmbassador::setshipNumber(const std::wstring& name) {
    shipNumber = name;
}

double MyShootShipFederateAmbassador::getshipheight() const {
    return shipheight;
}
void MyShootShipFederateAmbassador::setshipheight(const double& height) {
    shipheight = height;
}

double MyShootShipFederateAmbassador::getshipwidth() const {
    return shipwidth;
}
void MyShootShipFederateAmbassador::setshipwidth(const double& width) {
    shipwidth = width;
}

double MyShootShipFederateAmbassador::getshiplength() const {
    return shiplength;
}
void MyShootShipFederateAmbassador::setshiplength(const double& length) {
    shiplength = length;
}

double MyShootShipFederateAmbassador::getShipSize() {
    return shiplength * shipwidth * shipheight;
}

int MyShootShipFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MyShootShipFederateAmbassador::setNumberOfRobots(const int& numRobots) {
    numberOfRobots = numRobots;
}

rti1516e::ObjectClassHandle MyShootShipFederateAmbassador::getMyObjectClassHandle() const {
    return objectClassHandle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipFederateName() const {
    return attributeHandleMyShipFederateName;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipSpeed() const {
    return attributeHandleMyShipSpeed;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipPosition() const {
    return attributeHandleMyShipPosition;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleNumberOfRobots() const {
    return attributeHandleNumberOfRobots;
}