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
    std::wcout << L"[DEBUG] Recieve interaction called with time" << std::endl;
}

void MyShootShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Recieve interaction called without time" << std::endl;

    if (interactionClassHandle == setupSimulationHandle) {
        auto itBlueShips = parameterValues.find(blueShips);
        auto itRedShips = parameterValues.find(redShips);
        auto itTimeScaleFactor = parameterValues.find(timeScaleFactor);

        if (itBlueShips == parameterValues.end() 
        || itRedShips == parameterValues.end() 
        || itTimeScaleFactor == parameterValues.end()) {
            std::wcerr << L"Missing parameters in setup simulation interaction" << std::endl;
            return;
        }

        rti1516e::HLAinteger32BE paramValueBlueShips;
        paramValueBlueShips.decode(itBlueShips->second);
        std::wcout << L"Instance " << instance << L": Blue ships: " << paramValueBlueShips.get() << std::endl;

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << L"Instance " << instance << L": Red ships: " << paramValueRedShips.get() << std::endl;

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << L"Instance " << instance << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;


    }
}

void MyShootShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"Shooter Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"Master Federate synchronized at SimulationSetupComplete." << std::endl;
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

// Setters and getters for attribute handles
rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipPosition() const {
    return attributeHandleMyShipPosition;
}
void MyShootShipFederateAmbassador::setAttributeHandleMyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipPosition = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipFederateName() const {
    return attributeHandleMyShipFederateName;
}
void MyShootShipFederateAmbassador::setAttributeHandleMyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipFederateName = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleMyShipSpeed() const {
    return attributeHandleMyShipSpeed;
}
void MyShootShipFederateAmbassador::setAttributeHandleMyShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipSpeed = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleNumberOfRobots() const {
    return attributeHandleNumberOfRobots;
}
void MyShootShipFederateAmbassador::setAttributeHandleNumberOfRobots(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfRobots = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleEnemyShipFederateName() const {
    return attributeHandleEnemyShipFederateName;
}
void MyShootShipFederateAmbassador::setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipFederateName = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleEnemyShipPosition() const {
    return attributeHandleEnemyShipPosition;
}
void MyShootShipFederateAmbassador::setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipPosition = handle;
}

rti1516e::ObjectClassHandle MyShootShipFederateAmbassador::getMyObjectClassHandle() const {
    return objectClassHandle;
}
void MyShootShipFederateAmbassador::setMyObjectClassHandle(rti1516e::ObjectClassHandle handle) {
    objectClassHandle = handle;
}

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

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTargetPositionParam() const {
    return targetPosition;
}
void MyShootShipFederateAmbassador::setTargetPositionParam(const rti1516e::ParameterHandle& handle) {
    targetPosition = handle;
}

//Get and set for setup simulation interaction
rti1516e::InteractionClassHandle MyShootShipFederateAmbassador::getSetupSimulationHandle() const {
    return setupSimulationHandle;
}
void MyShootShipFederateAmbassador::setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle) {
    setupSimulationHandle = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getBlueShipsParam() const {
    return blueShips;
}
void MyShootShipFederateAmbassador::setBlueShipsParam(const rti1516e::ParameterHandle& handle) {
    blueShips = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getRedShipsParam() const {
    return redShips;
}
void MyShootShipFederateAmbassador::setRedShipsParam(const rti1516e::ParameterHandle& handle) {
    redShips = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTimeScaleFactorParam() const {
    return timeScaleFactor;
}
void MyShootShipFederateAmbassador::setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle) {
    timeScaleFactor = handle;
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