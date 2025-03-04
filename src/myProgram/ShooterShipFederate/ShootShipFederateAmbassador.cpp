#include "ShootShipFederateAmbassador.h"

MyShootShipFederateAmbassador::MyShootShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance) 
    : _rtiambassador(rtiAmbassador), instance(instance) {
    federateName = L"ShootShipFederate " + std::to_wstring(instance);
}

MyShootShipFederateAmbassador::~MyShootShipFederateAmbassador() {}

void MyShootShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}


void MyShootShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
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

int MyShootShipFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MyShootShipFederateAmbassador::setNumberOfRobots(const int& numRobots) {
    numberOfRobots = numRobots;
}

std::wstring MyShootShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void MyShootShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::wstring MyShootShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}
