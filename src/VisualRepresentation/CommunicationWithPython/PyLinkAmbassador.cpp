#include "PyLinkAmbassador.h"

PyLinkAmbassador::PyLinkAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiAmbassador(rtiAmbassador) {}

PyLinkAmbassador::~PyLinkAmbassador() {}

void PyLinkAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"EveryoneReady") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: EveryoneReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"ReadyToExit") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at ReadyToExit." << std::endl;
        syncLabel = label;
    }
}

void PyLinkAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"[DEBUG] Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass;
    std::wcout << L". Object name: " << theObjectName << std::endl;

    ships.emplace_back(theObject);
    shipMap[theObject] = ships.back();
}

void PyLinkAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    std::wcout << L"[DEBUG] Reflect attribute values called in object " << theObject << std::endl;
    auto itShip = shipMap.find(theObject);
    if (itShip == shipMap.end()) {
        std::wcerr << L"[ERROR] Object instance handle not found in shipMap" << std::endl;
        return;
    }

    Ship& ship = itShip->second;

    if (ship.shipTeam == L"Red") {
        std::wcout << L"[INFO - " << theObject << "] Ship belongs to the Red team." << std::endl;
        // Send to Python here
    } else if (ship.shipTeam == L"Blue") {
        std::wcout << L"[INFO - " << theObject << "] Ship belongs to the Blue team." << std::endl;
        // Send to Python here
    } else {
        std::wcout << L"[INFO - " << theObject << "] Ship team is unknown: " << ship.shipTeam << std::endl;
    }
}

void PyLinkAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void PyLinkAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void PyLinkAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    //std::wcout << L"[DEBUG] Time Advance Grant received: "
    //           << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;
    isAdvancing = false;
}

std::chrono::time_point<std::chrono::high_resolution_clock> PyLinkAmbassador::getStartTime() const {
    return startTime;
}
void PyLinkAmbassador::setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time) {
    startTime = time;
}
bool PyLinkAmbassador::getIsRegulating() const {
    return isRegulating;
}
bool PyLinkAmbassador::getIsConstrained() const {
    return isConstrained;
}
bool PyLinkAmbassador::getIsAdvancing() const {
    return isAdvancing;
}
void PyLinkAmbassador::setIsAdvancing(bool advancing) {
    isAdvancing = advancing;
}

std::wstring PyLinkAmbassador::getSyncLabel() const {
    return syncLabel;
}

std::vector<Ship>& PyLinkAmbassador::getShips() {
    return ships;
}
void PyLinkAmbassador::setShips(const std::vector<Ship>& ships) {
    this->ships = ships;
}

rti1516e::ObjectClassHandle PyLinkAmbassador::getObjectClassHandleShip() const {
    return objectClassHandleShip;
}
void PyLinkAmbassador::setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle) {
    objectClassHandleShip = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipID() const {
    return attributeHandleShipID;
}
void PyLinkAmbassador::setAttributeHandleShipID(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipID = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void PyLinkAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void PyLinkAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void PyLinkAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipSize() const {
    return attributeHandleShipSize;
}
void PyLinkAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSize = handle;
}
rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleShipHP() const {
    return attributeHandleShipHP;
}
void PyLinkAmbassador::setAttributeHandleShipHP(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipHP = handle;
}