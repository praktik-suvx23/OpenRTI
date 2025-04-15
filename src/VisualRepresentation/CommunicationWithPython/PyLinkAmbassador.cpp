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
    std::wcout << L"[DEBUG] Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass <<
    ". Number of found objects: " << numberOfDiscoveredObjects << std::endl;

    numberOfDiscoveredObjects++;
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
    //std::wcout << L"[DEBUG] Reflecting attribute values for object: " << theObject << std::endl;
    auto itShip = shipMap.find(theObject);
    if (itShip == shipMap.end()) {
        std::wcerr << L"[ERROR] Object not found in ship map: " << theObject << std::endl;
        return;
    }

    Ship& ship = itShip->second;
    // Extract attribute values
    for (const auto& [attributeHandle, value] : theAttributes) {
        try {
            if(attributeHandle == attributeHandleShipID) {
                rti1516e::HLAunicodeString shipID;
                shipID.decode(value);
                ship.shipName = shipID.get();
                //std::wcout << L"[INFO - " << theObject << "] Ship ID: " << ship.shipName << std::endl;
            } else if (attributeHandle == attributeHandleShipTeam) {
                rti1516e::HLAunicodeString shipTeam;
                shipTeam.decode(value);
                ship.shipTeam = shipTeam.get();
            } else if (attributeHandle == attributeHandleShipPosition) {
                std::pair<double, double> position = decodePositionRec(value);
                ship.shipPosition.first = position.first;
                ship.shipPosition.second = position.second;
            } else if (attributeHandle == attributeHandleShipSpeed) {
                rti1516e::HLAfloat64BE speed;
                speed.decode(value);
                ship.shipSpeed = speed.get();
            } else if (attributeHandle == attributeHandleShipSize) {
                rti1516e::HLAfloat64BE size;
                size.decode(value);
                ship.shipSize = size.get();
            } else if (attributeHandle == attributeHandleShipHP) {
                rti1516e::HLAfloat64BE hp;
                hp.decode(value);
                ship.shipHP = hp.get();
            }
        } catch (const rti1516e::Exception& e) {
            std::wcerr << L"[ERROR] Failed to decode attribute: " << e.what() << std::endl;
        }
    }

    if (ship.shipTeam == L"Red") {
        //std::wcout << L"[INFO - " << theObject << "] Ship belongs to the Red team." << std::endl;
        updateOrInsertShip(redShips, ship);
    } else if (ship.shipTeam == L"Blue") {
        //std::wcout << L"[INFO - " << theObject << "] Ship belongs to the Blue team." << std::endl;
        updateOrInsertShip(blueShips, ship);
    } else {
        std::wcout << L"[ERROR - " << theObject << "] Ship team is unknown: " << ship.shipTeam << std::endl;
    }
}

void PyLinkAmbassador::updateOrInsertShip(std::vector<Ship>& shipVec, Ship& ship) {
    auto it = std::find_if(shipVec.begin(), shipVec.end(), [&ship](const Ship& s) {
        return s.objectInstanceHandle == ship.objectInstanceHandle;
    });

    if (it != shipVec.end()) {
        *it = ship; // Update existing ship
    } else {
        shipVec.push_back(ship); // Insert new ship
    }
}

void PyLinkAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    //std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void PyLinkAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    //std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
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

std::vector<Ship>& PyLinkAmbassador::getRedShips() {
    return redShips;
}
void PyLinkAmbassador::clearRedShips() {
    redShips.clear();
}
std::vector<Ship>& PyLinkAmbassador::getBlueShips() {
    return blueShips;
}
void PyLinkAmbassador::clearBlueShips() {
    blueShips.clear();
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