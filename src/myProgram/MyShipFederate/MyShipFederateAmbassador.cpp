#include "MyShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    if (interactionClassHandle == hitEventHandle) {
        std::wcout << L"🚀 Ship has been hit! Processing HitEvent." << std::endl;

        std::wstring shipID;

        // May be needed if we implement a method where
        // ships want to know which ships are left on the field.
        auto iterShip = parameterValues.find(shipIDParam);
        if (iterShip != parameterValues.end()) {
            rti1516e::HLAunicodeString shipIDDecoder;
            shipIDDecoder.decode(iterShip->second);
            shipID = shipIDDecoder.get();
            if(shipID != federateName) {
                return;
            }
        }

        auto iterRobot = parameterValues.find(robotIDParam);
        if (iterRobot != parameterValues.end()) {
            rti1516e::HLAunicodeString robotIDDecoder;
            robotIDDecoder.decode(iterRobot->second);
            robotID = robotIDDecoder.get();
        }

        // This is template. Make something cool with it later.
        auto iterDamage = parameterValues.find(damageParam);
        if (iterDamage != parameterValues.end()) {
            rti1516e::HLAinteger32BE damageDecoder;
            damageDecoder.decode(iterDamage->second);
            damageAmount = damageDecoder.get();
            hitStatus = true;
        }

        std::wcout << L"💥 Ship " << shipID << L" was hit by Robot " << robotID 
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