#include "MyShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    // Look for the "ShipLocked" attribute:
    auto itShipLocked = theAttributes.find(attributeHandleShipLocked);

    if (itShipLocked != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueShipLocked;
        attributeValueShipLocked.decode(itShipLocked->second);
        std::wstring lockedByRobot = attributeValueShipLocked.get();

        std::wcout << L"Ship locked by: " << lockedByRobot << std::endl;

        // If the ship is locked, do something with that info
        if (lockedByRobot != L"EMPTY") {
            // Take action based on the ship being locked
            std::wcout << L"Ship is now locked by robot: " << lockedByRobot << std::endl;
        }
    }
}

void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
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

        // Handle damage
        auto iterDamage = parameterValues.find(damageParam);
        if (iterDamage != parameterValues.end()) {
            rti1516e::HLAinteger32BE damageDecoder;
            damageDecoder.decode(iterDamage->second);
            damageAmount = damageDecoder.get();
            //Remove later when better logic is implemented
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

std::wstring MyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

std::wstring MyShipFederateAmbassador::getFederateName() const {
    return federateName;
}

void MyShipFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

bool MyShipFederateAmbassador::getHitStatus() const {
    return hitStatus;
}

std::wstring MyShipFederateAmbassador::getRobotID() const {
    return robotID;
}

int MyShipFederateAmbassador::getDamageAmount() const {
    return damageAmount;
}