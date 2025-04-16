#include "ShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiambassador(rtiAmbassador)  {
}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
        std::wcout << L"[DEBUG] Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass <<
        ". Number of found objects: " << numberOfDiscoveredObjects << std::endl;

    numberOfDiscoveredObjects++;
    shipMap[theObject] = Ship(theObject);
}

void MyShipFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    if (shipMap.find(theObject) == shipMap.end()) {
        std::wcerr << L"[ERROR] Object not found in ship map: " << theObject << std::endl;
        return;
    }

    auto& ship = shipMap[theObject];
    Ship* shipPtr = &ship;

    bool isInFriendly = std::find(friendlyShips.begin(), friendlyShips.end(), shipPtr) != friendlyShips.end();
    bool isInEnemy = std::find(enemyShips.begin(), enemyShips.end(), shipPtr) != enemyShips.end();

    if (!isInFriendly && !isInEnemy) {
        auto itShipTeam = theAttributes.find(attributeHandleShipTeam);
        if (itShipTeam != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueShipTeam;
            attributeValueShipTeam.decode(itShipTeam->second);
            ship.shipTeam = attributeValueShipTeam.get();

            bool isBlueFederate = federateName[0] == L'B';
            bool isFriendly = (isBlueFederate && ship.shipTeam == L"Blue") ||
                            (!isBlueFederate && ship.shipTeam == L"Red");

            if (isFriendly) {
                friendlyShips.push_back(shipPtr);
                std::wcout << L"[INFO] Friendly ship found" << std::endl;
            } else {
                enemyShips.push_back(shipPtr);
                std::wcout << L"[INFO] Enemy ship assigned" << std::endl;
            }
        }
    }

    if (ship.shipName == L"") {
        const auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
        if (itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itShipFederateName->second);
            ship.shipName = attributeValueFederateName.get();
            std::wcout << L"[INFO] Updated target federate name: " << ship.shipName << std::endl;
        } else {
            std::wcerr << L"[INFO] Attribute handle for ship federate name not found" << std::endl;
        }
    }
    const auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
    if (itShipPosition != theAttributes.end()) {
        std::pair<double, double> tempShipPosition = decodePositionRec(itShipPosition->second);
        ship.shipPosition = tempShipPosition;
        std::wcout << L"[INFO] Updated target ship position: " << ship.shipPosition.first << L", " << ship.shipPosition.second << L" for the object" << theObject << std::endl;
    } else {
        std::wcerr << L"[INFO] Attribute handle for ship position not found" << std::endl;
    }
}

//TSO (Time Stamp Order) 
//This function is called when an interaction is received with a time stamp
void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {

    std::wcout << L"[DEBUG] Receive interaction called with time" << std::endl;

    if (interactionClassHandle == interactionClassTargetHit) {
        auto it = parameterValues.find(parameterHandleTargetHitID);
        if (it != parameterValues.end()) {
            rti1516e::VariableLengthData attributeValue = it->second;
            rti1516e::HLAunicodeString value;
            value.decode(attributeValue);
            std::wstring targetID = value.get();
            std::wcout << L"[INFO] Target ID: " << targetID << std::endl;

            for (auto it = friendlyShips.begin(); it != friendlyShips.end(); ) {
                Ship* ship = *it;

                if (ship->shipName == targetID) {
                    ship->shipHP -= 50; // Apply damage

                    if (ship->shipHP <= 0) {
                        std::wcout << L"[DESTROYED] Ship: " << ship->shipName << std::endl;

                        // Remove ship from friendlyShips list
                        it = friendlyShips.erase(it);
                        // Optional: remove from shipMap if you want full deletion
                        // shipMap.erase(ship->objectInstanceHandle);

                        continue; // Skip incrementing
                    } else {
                        std::wcout << L"[HIT] Ship: " << ship->shipName << L", HP now: " << ship->shipHP << std::endl;
                    }
                }

                ++it;
            }
        }
    }
}

// RO (Receive Order)
// This function is called when an interaction is received without a time stamp
void MyShipFederateAmbassador::receiveInteraction(
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
        std::wcout << L": Blue ships: " << paramValueBlueShips.get() << std::endl;
        std::wcout << federateName << std::endl;
        if (federateName.find(L"BlueShipFederate") ==  0) {
            std::wcout << L"Creating blue ships" << std::endl;
            createNewShips(paramValueBlueShips.get());
        }

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << std::endl << L": Red ships: " << paramValueRedShips.get() << std::endl;
        std::wcout << federateName << std::endl;
        if (federateName.find(L"RedShipFederate") == 0) {
            std::wcout << std::endl << L"Creating red ships" << std::endl;
            createNewShips(paramValueRedShips.get());
        }

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << std::endl << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
        timeScale = paramValueTimeScaleFactor.get();
    }
}

void MyShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"[INFO - SyncPoint] Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label.find(L"BlueShipFederate") == 0) {
        std::wcout << L"Federate synchronized at BlueTeamSync." << std::endl;
        blueSyncLabel = L"BlueShipFederate";
    }
    if (label.find(L"RedShipFederate") == 0) {
        std::wcout << L"Federate synchronized at RedTeamSync." << std::endl;
        redSyncLabel = L"RedShipFederate";
    }
    if (label == L"MissileReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at MissileReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at RedShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at BlueShipReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"EveryoneReady") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at EveryoneReady." << std::endl;
        syncLabel = label;
    }
    if (label == L"ReadyToExit") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at ReadyToExit." << std::endl;
        syncLabel = label;
    }
    if (label == L"RedShipEmpty") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at RedShipEmpty." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipEmpty") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at BlueShipEmpty." << std::endl;
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

void MyShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    //std::wcout << L"[DEBUG] Time Advance Grant received: "
    //           << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

void MyShipFederateAmbassador::createNewShips(int amountOfShips) {
    try {
        if (amountOfShips <= 0) {
            std::wcerr << L"[ERROR] Invalid number of ships to create: " << amountOfShips << std::endl;
            return;
        }

        std::wcout << L"[INFO] Creating " << amountOfShips << L" ship(s)" << std::endl;

        int maxShipsPerRow = getOptimalShipsPerRow(amountOfShips);
        std::pair<double, double> baseShipPosition = {20.43829000, 15.62534000};

        for (int i = 0; i < amountOfShips; ++i) {
            int row = i / maxShipsPerRow;
            int column = i % maxShipsPerRow;

            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(objectClassHandleShip);

            // Add Ship to shipMap
            shipMap[objectInstanceHandle] = Ship(objectInstanceHandle);
            Ship* ship = &shipMap[objectInstanceHandle];

            // Push the pointer to friendlyShips
            friendlyShips.push_back(ship);

            // Assign ship name and team
            if (federateName.find(L"BlueShipFederate") == 0) {
                ship->shipName = L"BlueShip_" + std::to_wstring(getpid()) + L"_" + std::to_wstring(shipCounter++);
                ship->shipTeam = L"Blue";
            } else if (federateName.find(L"RedShipFederate") == 0) {
                ship->shipName = L"RedShip_" + std::to_wstring(getpid()) + L"_" + std::to_wstring(shipCounter++);
                ship->shipTeam = L"Red";
            }

            // Assign position
            ship->shipPosition = generateDoubleShipPosition(baseShipPosition, ship->shipTeam, row, column);

            // Read any additional config (missiles etc.)
            readJsonFile();

            // Construct position record
            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.second));

            std::wcout << L"[INFO] Registered ship: " << ship->shipName << std::endl;

            // Construct attribute map
            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandleShipFederateName] = rti1516e::HLAunicodeString(ship->shipName).encode();
            attributes[attributeHandleShipPosition] = shipPositionRecord.encode();
            attributes[attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[attributeHandleNumberOfMissiles] = rti1516e::HLAinteger32BE(ship->shipNumberOfMissiles).encode();
            // TODO: Add numberOfCanons if needed

            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }

        createShips = true;

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[EXCEPTION] " << e.what() << std::endl;
    }
}

void MyShipFederateAmbassador::readJsonFile() {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);

    if (!parser.isFileOpen()) return;

    int i = dis(gen); // Randomly select a ship configuration
    parser.parseShipConfig("Ship" + std::to_string(i));

    // Access the last friendly ship pointer
    if (friendlyShips.empty()) return; // safety check
    Ship* ship = friendlyShips.back();

    ship->shipSize = parser.getShipSize();
    ship->shipNumberOfMissiles = parser.getNumberOfMissiles();
    ship->shipNumberOfCanons = parser.getNumberOfCanons();

    std::wcout << std::endl
               << L"Ship size: " << ship->shipSize << L" for ship " << ship->shipName << std::endl
               << L"Number of missiles: " << ship->shipNumberOfMissiles << L" for ship " << ship->shipName << std::endl
               << L"Number of canons: " << ship->shipNumberOfCanons << L" for ship " << ship->shipName << std::endl;
}

void MyShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    // Create a new Ship object and add it to the shipMap (which owns the Ship)
    Ship newShip(objectHandle);
    shipMap[objectHandle] = std::move(newShip); // Emplace into map (copy or move)

    // Get a pointer to the stored Ship and add it to friendlyShips
    Ship* shipPtr = &shipMap[objectHandle];
    friendlyShips.push_back(shipPtr);
}


void MyShipFederateAmbassador::setFederateName(const std::wstring& name) {
    federateName = name;
}
std::wstring MyShipFederateAmbassador::getFederateName() const{
    return federateName;
}

// Getter and setter for Object Class Ship and its attributes
rti1516e::ObjectClassHandle MyShipFederateAmbassador::getObjectClassHandleShip() const {
    return objectClassHandleShip;
}
void MyShipFederateAmbassador::setObjectClassHandleShip(rti1516e::ObjectClassHandle handle) {
    objectClassHandleShip = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipFederateName() const {
    return attributeHandleShipFederateName;
}
void MyShipFederateAmbassador::setAttributeHandleShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void MyShipFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MyShipFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MyShipFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void MyShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleEnemyShipFederateName() const {
    return attributeHandleEnemyShipFederateName;
}
void MyShipFederateAmbassador::setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipFederateName = handle;
}

rti1516e::AttributeHandle MyShipFederateAmbassador::getAttributeHandleEnemyShipPosition() const {
    return attributeHandleEnemyShipPosition;
}
void MyShipFederateAmbassador::setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipPosition = handle;
}

// Getter and setter for interaction class SetupSimulation and its parameters
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getSetupSimulationHandle() const {
    return setupSimulationHandle;
}
void MyShipFederateAmbassador::setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle) {
    setupSimulationHandle = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getBlueShipsParam() const {
    return blueShips;
}
void MyShipFederateAmbassador::setBlueShipsParam(const rti1516e::ParameterHandle& handle) {
    blueShips = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getRedShipsParam() const {
    return redShips;
}
void MyShipFederateAmbassador::setRedShipsParam(const rti1516e::ParameterHandle& handle) {
    redShips = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getTimeScaleFactorParam() const {
    return timeScaleFactor;
}
void MyShipFederateAmbassador::setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle) {
    timeScaleFactor = handle;
}

// Getters and setters for ship attributes

std::wstring MyShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void MyShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::pair<double, double> MyShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void MyShipFederateAmbassador::setEnemyShipPosition(const std::pair<double, double>& position) {
    enemyShipPosition = position;
}

double MyShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void MyShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double MyShipFederateAmbassador::getBearing() const {
    return bearing;
}
void MyShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool MyShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void MyShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring MyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MyShipFederateAmbassador::getRedSyncLabel() const {
    return redSyncLabel;
}
std::wstring MyShipFederateAmbassador::getBlueSyncLabel() const {
    return blueSyncLabel;
}

// Getter and setter for createShipsSyncPoint
bool MyShipFederateAmbassador::getCreateShips() const {
    return createShips;
}

// Getter and setter functions for interaction class FireMissile
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassFireMissile() const {
    return interactionClassFireMissile;
}
void MyShipFederateAmbassador::setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle) {
    interactionClassFireMissile = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}
void MyShipFederateAmbassador::setParamShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleShooterID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}
void MyShipFederateAmbassador::setParamMissileTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTeam = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}
void MyShipFederateAmbassador::setParamMissileStartPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileStartPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}
void MyShipFederateAmbassador::setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTargetPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MyShipFederateAmbassador::setParamNumberOfMissilesFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleNumberOfMissilesFired = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
void MyShipFederateAmbassador::setParamMissileSpeed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileSpeed = handle;
}

// Getter and setter functions for interaction class targetHit
rti1516e::InteractionClassHandle MyShipFederateAmbassador::getInteractionClassTargetHit() const {
    return interactionClassTargetHit;
}
void MyShipFederateAmbassador::setInteractionClassTargetHit(const rti1516e::InteractionClassHandle& handle) {
    interactionClassTargetHit = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitID() const {
    return parameterHandleTargetHitID;
}
void MyShipFederateAmbassador::setParamTargetHitID(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitID = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitTeam() const {
    return parameterHandleTargetHitTeam;
}
void MyShipFederateAmbassador::setParamTargetHitTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitTeam = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitPosition() const {
    return parameterHandleTargetHitPosition;
}
void MyShipFederateAmbassador::setParamTargetHitPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitPosition = handle;
}

rti1516e::ParameterHandle MyShipFederateAmbassador::getParamTargetHitDestroyed() const {
    return parameterHandleTargetHitDestroyed;
}
void MyShipFederateAmbassador::setParamTargetHitDestroyed(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitDestroyed = handle;
}

// Getter and setter for time management
bool MyShipFederateAmbassador::getIsRegulating() const {
    return isRegulating;
}
bool MyShipFederateAmbassador::getIsConstrained() const {
    return isConstrained;
}
bool MyShipFederateAmbassador::getIsAdvancing() const {
    return isAdvancing;
}
void MyShipFederateAmbassador::setIsAdvancing(bool advancing) {
    isAdvancing = advancing;
}

// Getter and setter for shipMap
std::unordered_map<rti1516e::ObjectInstanceHandle, Ship>& MyShipFederateAmbassador::getShipMap() {
    return shipMap;
}
std::vector<Ship*>& MyShipFederateAmbassador::getFriendlyShips() {
    return friendlyShips;
}
std::vector<Ship*>& MyShipFederateAmbassador::getEnemyShips() {
    return enemyShips;
}
