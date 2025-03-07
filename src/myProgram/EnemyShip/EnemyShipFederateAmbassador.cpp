#include "EnemyShipFederateAmbassador.h"

EnemyShipFederateAmbassador::EnemyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance) 
    : _rtiambassador(rtiAmbassador), instance(instance) {
    _expectedShipName = L"ShootShipFederate " + std::to_wstring(instance);
}

EnemyShipFederateAmbassador::~EnemyShipFederateAmbassador() {}

void EnemyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void EnemyShipFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    std::wcout << L"[DEBUG] Reflect attribute values called" << std::endl;

    //Calculate distance between ships
    auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
    auto itEnemyShipFederateName = theAttributes.find(attributeHandleEnemyShipFederateName);

    if (itEnemyShipPosition != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueEnemyShipPosition;
        attributeValueEnemyShipPosition.decode(itEnemyShipPosition->second);
        setEnemyShipPosition(attributeValueEnemyShipPosition.get());
    }
    if (itEnemyShipFederateName != theAttributes.end()) { //Use this to start a robot to shoot at this federateName 
        rti1516e::HLAunicodeString attributeValueEnemyShipFederateName;
        attributeValueEnemyShipFederateName.decode(itEnemyShipFederateName->second);
        setEnemyShipFederateName(attributeValueEnemyShipFederateName.get());
    }
}


void EnemyShipFederateAmbassador::receiveInteraction(
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

void EnemyShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"Shooter Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
}

void EnemyShipFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void EnemyShipFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void EnemyShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

// Setters and getters for attribute handles
rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipPosition() const {
    return attributeHandleMyShipPosition;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipPosition = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipFederateName() const {
    return attributeHandleMyShipFederateName;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipFederateName = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipSpeed() const {
    return attributeHandleMyShipSpeed;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipSpeed = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleNumberOfRobots() const {
    return attributeHandleNumberOfRobots;
}
void EnemyShipFederateAmbassador::setAttributeHandleNumberOfRobots(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfRobots = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleEnemyShipFederateName() const {
    return attributeHandleEnemyShipFederateName;
}
void EnemyShipFederateAmbassador::setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipFederateName = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleEnemyShipPosition() const {
    return attributeHandleEnemyShipPosition;
}
void EnemyShipFederateAmbassador::setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipPosition = handle;
}

rti1516e::ObjectClassHandle EnemyShipFederateAmbassador::getMyObjectClassHandle() const {
    return objectClassHandle;
}
void EnemyShipFederateAmbassador::setMyObjectClassHandle(rti1516e::ObjectClassHandle handle) {
    objectClassHandle = handle;
}

//Get and set for fire interaction
rti1516e::InteractionClassHandle EnemyShipFederateAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void EnemyShipFederateAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void EnemyShipFederateAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

// Getters and setters for ship attributes
std::wstring EnemyShipFederateAmbassador::getMyShipPosition() const {
    return myShipPosition;
}
void EnemyShipFederateAmbassador::setMyShipPosition(const std::wstring& position) {
    myShipPosition = position;
}

std::wstring EnemyShipFederateAmbassador::getMyShipFederateName() const {
    return myShipFederateName;
}
void EnemyShipFederateAmbassador::setMyShipFederateName(const std::wstring& name) {
    myShipFederateName = name;
}

double EnemyShipFederateAmbassador::getMyShipSpeed() const {
    return myShipSpeed;
}
void EnemyShipFederateAmbassador::setMyShipSpeed(const double& speed) {
    myShipSpeed = speed;
}

std::wstring EnemyShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void EnemyShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::wstring EnemyShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void EnemyShipFederateAmbassador::setEnemyShipPosition(const std::wstring& position) {
    enemyShipPosition = position;
}

double EnemyShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void EnemyShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double EnemyShipFederateAmbassador::getBearing() const {
    return bearing;
}
void EnemyShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool EnemyShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void EnemyShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring EnemyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

//Json values get/set
std::wstring EnemyShipFederateAmbassador::getshipNumber() const {
    return shipNumber;
}
void EnemyShipFederateAmbassador::setshipNumber(const std::wstring& name) {
    shipNumber = name;
}

double EnemyShipFederateAmbassador::getshipheight() const {
    return shipheight;
}
void EnemyShipFederateAmbassador::setshipheight(const double& height) {
    shipheight = height;
}

double EnemyShipFederateAmbassador::getshipwidth() const {
    return shipwidth;
}
void EnemyShipFederateAmbassador::setshipwidth(const double& width) {
    shipwidth = width;
}

double EnemyShipFederateAmbassador::getshiplength() const {
    return shiplength;
}
void EnemyShipFederateAmbassador::setshiplength(const double& length) {
    shiplength = length;
}

double EnemyShipFederateAmbassador::getShipSize() {
    ShipSize = shiplength * shipwidth * shipheight;
    return ShipSize;
}

void EnemyShipFederateAmbassador::setNumberOfRobots(const int& numRobots) {
    numberOfRobots = numRobots;
}
int EnemyShipFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}

