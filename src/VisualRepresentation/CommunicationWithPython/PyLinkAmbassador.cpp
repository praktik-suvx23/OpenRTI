#include "PyLinkAmbassador.h"

PyLinkAmbassador::PyLinkAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiAmbassador(rtiAmbassador) {}

PyLinkAmbassador::~PyLinkAmbassador() {}

void PyLinkAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"InitialSync") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"EveryoneReady") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: EveryoneReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"ReadyToExit") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: ReadyToExit." << std::endl;
        syncLabel = label;
    }
    if (label == L"NoMessagesReceived") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: NoMessagesReceived." << std::endl;
        syncLabel = label;
    }
}

void PyLinkAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {

    try {
        discoveredObjects[theObject] = numberOfDiscoveredObjects;
        numberOfDiscoveredObjects++;

        logWmessage = L"[NEW OBJECT " + std::to_wstring(numberOfDiscoveredObjects) + 
            L"] ObjectInstanceHandle: " + std::to_wstring(theObject.hash());
        wstringToLog(logWmessage, logType);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Exception in discoverObjectInstance: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"[ERROR] Exception in discoverObjectInstance: " << e.what() << std::endl;
    }
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
    if (theTag.size() > 0) {
        std::string tagStr(reinterpret_cast<const char*>(theTag.data()), theTag.size());
        std::wstring tagWStr(tagStr.begin(), tagStr.end());

        if (tagWStr == L"Missile") {
            try {
            // Handle missile-specific update
            const auto itMissileID = theAttributes.find(attributeHandleMissileID);
            const auto itMissileTeam = theAttributes.find(attributeHandleMissileTeam);
            const auto itMissilePosition = theAttributes.find(attributeHandleMissilePosition);
            const auto itMissileAltitude = theAttributes.find(attributeHandleMissileAltitude);
            const auto itMissileSpeed = theAttributes.find(attributeHandleMissileSpeed);

            bool isMissile = (itMissileID != theAttributes.end()
                && itMissileTeam != theAttributes.end()
                && itMissilePosition != theAttributes.end()
                && itMissileAltitude != theAttributes.end());
            
                if (isMissile) {
                    rti1516e::HLAunicodeString tempString;
                    rti1516e::HLAfloat64BE tempFloat;
                    Missile missile;

                    if (missile.objectInstanceHandle == rti1516e::ObjectInstanceHandle()) {
                        missile.objectInstanceHandle = theObject;
                    }

                    tempString.decode(itMissileID->second);
                    missile.id = tempString.get();
                    tempString.decode(itMissileTeam->second);
                    missile.team = tempString.get();
                    std::pair<double, double> position = decodePositionRec(itMissilePosition->second);
                    missile.position = position;
                    tempFloat.decode(itMissileAltitude->second);
                    missile.altitude = tempFloat.get();

                    updateOrInsertMissile(missiles, missile);
                } else {
                    std::wstring errorMessage;
                    errorMessage = (itMissileID == theAttributes.end()) ? L"Missile ID not found. " : L"";
                    errorMessage += (itMissileTeam == theAttributes.end()) ? L"Missile team not found. " : L"";
                    errorMessage += (itMissilePosition == theAttributes.end()) ? L"Missile position not found. " : L"";
                    errorMessage += (itMissileAltitude == theAttributes.end()) ? L"Missile altitude not found. " : L"";
                    errorMessage += (itMissileSpeed == theAttributes.end()) ? L"Missile speed not found. " : L"";
                    std::wcerr << L"[ERROR - " << theObject << L"] " << errorMessage << std::endl;
                    return;
                }
            } catch (const rti1516e::Exception& e) {
                std::wcerr << L"[ERROR-M] Exception in reflectAttributeValues: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::wcerr << L"[ERROR-M] Exception in reflectAttributeValues: " << e.what() << std::endl;
            }
        } else if (tagWStr == L"Ship") {
            try {
                // Handle ship-specific update
                const auto itShipID = theAttributes.find(attributeHandleShipID);
                const auto itShipTeam = theAttributes.find(attributeHandleShipTeam);
                const auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
                const auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
                const auto itShipSize = theAttributes.find(attributeHandleShipSize);
                const auto itShipHP = theAttributes.find(attributeHandleShipHP);

                bool isShip = (itShipID != theAttributes.end()
                    && itShipTeam != theAttributes.end()
                    && itShipPosition != theAttributes.end()
                    && itShipSpeed != theAttributes.end());

                if (isShip) {
                    rti1516e::HLAunicodeString tempString;
                    rti1516e::HLAfloat64BE tempFloat;
                    Ship ship;

                    if (ship.objectInstanceHandle == rti1516e::ObjectInstanceHandle()) {
                        ship.objectInstanceHandle = theObject;
                    } 

                    tempString.decode(itShipID->second);
                    ship.shipName = tempString.get();
                    tempString.decode(itShipTeam->second);
                    ship.shipTeam = tempString.get();
                    std::pair<double, double> position = decodePositionRec(itShipPosition->second);
                    ship.shipPosition = position;
                    tempFloat.decode(itShipSpeed->second);
                    ship.shipSpeed = tempFloat.get();

                    if (ship.shipTeam == L"Red") {
                        updateOrInsertShip(redShips, ship);
                    } else if (ship.shipTeam == L"Blue") {
                        updateOrInsertShip(blueShips, ship);
                    } else {
                        std::wcout << L"[ERROR - " << theObject << "] Ship team is unknown: " << ship.shipTeam << std::endl;
                    }
                } else if (!isShip) {
                    std::wstring errorMessage;
                    errorMessage = (itShipID == theAttributes.end()) ? L"Ship ID not found. " : L"";
                    errorMessage += (itShipTeam == theAttributes.end()) ? L"Ship team not found. " : L"";
                    errorMessage += (itShipPosition == theAttributes.end()) ? L"Ship position not found. " : L"";
                    errorMessage += (itShipSpeed == theAttributes.end()) ? L"Ship speed not found. " : L"";
                    errorMessage += (itShipSize == theAttributes.end()) ? L"Ship size not found. " : L"";
                    errorMessage += (itShipHP == theAttributes.end()) ? L"Ship HP not found. " : L"";
                    std::wcerr << L"[ERROR - " << theObject << L"] " << errorMessage << std::endl;
                    return;
                }
            } catch (const rti1516e::Exception& e) {
                std::wcerr << L"[ERROR-S] Exception in reflectAttributeValues: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::wcerr << L"[ERROR-S] Exception in reflectAttributeValues: " << e.what() << std::endl;
            }
        } else {
            std::wcerr << L"[ERROR] Unknown tag received: " << tagWStr << std::endl;
        }
    } else {
        std::wcerr << L"[ERROR] Received empty tag." << std::endl;
    }
}

void PyLinkAmbassador::updateOrInsertShip(std::vector<Ship>& shipVec, Ship& ship) {
    auto it = std::find_if(shipVec.begin(), shipVec.end(), [&ship](const Ship& s) {
        return s.objectInstanceHandle == ship.objectInstanceHandle;
    });

    if (it != shipVec.end()) {
        *it = ship;
    } else {
        shipVec.push_back(ship);
    }
}

void PyLinkAmbassador::updateOrInsertMissile(std::vector<Missile>& missileVec, Missile& missile) {
    auto it = std::find_if(missileVec.begin(), missileVec.end(), [&missile](const Missile& m) {
        return m.objectInstanceHandle == missile.objectInstanceHandle;
    });

    if (it != missileVec.end()) {
        *it = missile;
    } else {
        missileVec.push_back(missile);
    }
}

void PyLinkAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
}

void PyLinkAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
}

void PyLinkAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
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
std::vector<Ship>& PyLinkAmbassador::getBlueShips() {
    return blueShips;
}
std::vector<Missile>& PyLinkAmbassador::getMissiles() {
    return missiles;
}

loggingType PyLinkAmbassador::getLogType() const {
    return logType;
}
void PyLinkAmbassador::setLogType(loggingType newType) {
    logType = newType;
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

// Getters and setters Object Class Missile and its attributes
rti1516e::ObjectClassHandle PyLinkAmbassador::getObjectClassHandleMissile() const {
    return objectClassHandleMissile;
}
void PyLinkAmbassador::setObjectClassHandleMissile(const rti1516e::ObjectClassHandle& handle) {
    objectClassHandleMissile = handle;
}

rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleMissileID() const {
    return attributeHandleMissileID;
}
void PyLinkAmbassador::setAttributeHandleMissileID(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileID = handle;
}

rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleMissileTeam() const {
    return attributeHandleMissileTeam;
}
void PyLinkAmbassador::setAttributeHandleMissileTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileTeam = handle;
}

rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleMissilePosition() const {
    return attributeHandleMissilePosition;
}
void PyLinkAmbassador::setAttributeHandleMissilePosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissilePosition = handle;
}

rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleMissileAltitude() const {
    return attributeHandleMissileAltitude;
}
void PyLinkAmbassador::setAttributeHandleMissileAltitude(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileAltitude = handle;
}

rti1516e::AttributeHandle PyLinkAmbassador::getAttributeHandleMissileSpeed() const {
    return attributeHandleMissileSpeed;
}
void PyLinkAmbassador::setAttributeHandleMissileSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileSpeed = handle;
}