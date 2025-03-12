#include "MyShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}



void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] 1" << std::endl;
}

void MyShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
}

void MyShipFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MyShipFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MyShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) {
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

//getter and setter for object class Ship and its attributes
rti1516e::ObjectClassHandle MyShipFederateAmbassador::getObjectClassHandleShip() const {return shipClassHandle;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipID() const {return attributeHandleShipID;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipTeam() const {return attributeHandleShipTeam;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipPosition() const {return attributeHandleShipPosition;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipSpeed() const {return attributeHandleShipSpeed;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipSize() const {return attributeHandleShipSize;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipAngle() const {return attributeHandleShipAngle;}
rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {return attributeHandleNumberOfMissiles;}
void MyShipFederateAmbassador::setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle) {shipClassHandle = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipID(const rti1516e::AttributeHandle& handle) {attributeHandleShipID = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {attributeHandleShipTeam = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {attributeHandleShipPosition = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {attributeHandleShipSpeed = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {attributeHandleShipSize = handle;}
void MyShipFederateAmbassador::setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle) {attributeHandleShipAngle = handle;}
void MyShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {attributeHandleNumberOfMissiles = handle;}

// Setters and getters for ship attributes
void MyShipFederateAmbassador::setshipNumber(std::wstring name) {
    shipNumber = name;
}
std::wstring MyShipFederateAmbassador::getshipNumber() const {
    return shipNumber;
}
void MyShipFederateAmbassador::setshipheight(double height){
    shipheight = height;
}
double MyShipFederateAmbassador::getshipheight() const {
    return shipheight;
}
void MyShipFederateAmbassador::setshipwidth(double width) {
    shipwidth = width;
}
double MyShipFederateAmbassador::getshipwidth() const {
    return shipwidth;
}
void MyShipFederateAmbassador::setshiplength(double length) {
    shiplength = length;
}
double MyShipFederateAmbassador::getshiplength() const {
    return shiplength;
}
double MyShipFederateAmbassador::getShipSize() {
    ShipSize = shiplength * shipwidth * shipheight;
    return ShipSize;
}
void MyShipFederateAmbassador::setNumberOfRobots(int numRobots) {
    numberOfRobots = numRobots;
}
int MyShipFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}   


// Setter for federate name and getter for sync label
void MyShipFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}
std::wstring MyShipFederateAmbassador::getFederateName() const {
    return federateName;
}

std::wstring MyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

// Used in 'registerShipObject'
rti1516e::ObjectClassHandle MyShipFederateAmbassador::getMyObjectClassHandle() const {return objectClassHandle;}
void MyShipFederateAmbassador::setMyObjectClassHandle(rti1516e::ObjectClassHandle handle) {objectClassHandle = handle;}