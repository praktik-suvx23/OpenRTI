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
rti1516e::ObjectClassHandle MissileManagerAmbassador::getObjectClassHandleShip() const {return shipClassHandle;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipID() const {return attributeHandleShipID;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipTeam() const {return attributeHandleShipTeam;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipPosition() const {return attributeHandleShipPosition;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSpeed() const {return attributeHandleShipSpeed;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSize() const {return attributeHandleShipSize;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipAngle() const {return attributeHandleShipAngle;}
rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleNumberOfMissiles() const {return attributeHandleNumberOfMissiles;}
void MissileManagerAmbassador::setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle) {shipClassHandle = handle;}
void MissileManagerAmbassador::setAttributeHandleShipID(const rti1516e::AttributeHandle& handle) {attributeHandleShipID = handle;}
void MissileManagerAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {attributeHandleShipTeam = handle;}
void MissileManagerAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {attributeHandleShipPosition = handle;}
void MissileManagerAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {attributeHandleShipSpeed = handle;}
void MissileManagerAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {attributeHandleShipSize = handle;}
void MissileManagerAmbassador::setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle) {attributeHandleShipAngle = handle;}
void MissileManagerAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {attributeHandleNumberOfMissiles = handle;}

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

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getstartPosRobot() const {
    return startPosRobot;
}
void MyShootShipFederateAmbassador::setstartPosRobot(const rti1516e::ParameterHandle& handle) {
    startPosRobot = handle;
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