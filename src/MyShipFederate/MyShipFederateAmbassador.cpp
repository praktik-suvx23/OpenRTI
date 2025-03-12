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

    if (interactionClassHandle == hitEventHandle) {
        std::wcout << L"Ship has been hit! Processing HitEvent." << std::endl;

        std::wstring shipID;

        // Ship ID handling
        auto iterShip = parameterValues.find(shipIDParam);
        if (iterShip != parameterValues.end()) {
            rti1516e::HLAunicodeString shipIDDecoder;
            shipIDDecoder.decode(iterShip->second);
            shipID = shipIDDecoder.get();
            if(shipID != federateName) {
                return;
            }
        }

        // Robot ID handling
        auto iterRobot = parameterValues.find(robotIDParam);
        if (iterRobot != parameterValues.end()) {
            rti1516e::HLAunicodeString robotIDDecoder;
            robotIDDecoder.decode(iterRobot->second);
            robotID = robotIDDecoder.get();
        }

        // Handle damage again if needed (it seems like you are repeating this logic)
        auto iterDamage = parameterValues.find(damageParam);
        if (iterDamage != parameterValues.end()) {
            rti1516e::HLAinteger32BE damageDecoder;
            damageDecoder.decode(iterDamage->second);
            damageAmount = damageDecoder.get();
            if(damageAmount > 10) 
                hitStatus = true;
        }

        std::wcout << L"Ship " << shipID << L" was hit by Robot " << robotID 
                   << L" for " << damageAmount << L" damage!" << std::endl;
    }
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