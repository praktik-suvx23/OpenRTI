#include "ShipFederateAmbassador.h"

MyShipFederateAmbassador::MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiambassador(rtiAmbassador)  {
}

MyShipFederateAmbassador::~MyShipFederateAmbassador() {}

void MyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;

    enemyShips.emplace_back(theObject);
    enemyShipIndexMap[theObject] = enemyShips.size() - 1;
    std::wcout << L"Enemy ship instance handle: " << theObject << std::endl;
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
        //Debugging for attribute values and map
        std::wcout << L"-------------------------------------------------------------" << std::endl;
        std::wcout << L"[DEBUG] Reflect attribute values called in object "<< theObject << std::endl;
        auto itEnemyShip = enemyShipIndexMap.find(theObject);
        if (itEnemyShip == enemyShipIndexMap.end()) {
            std::wcerr << L"Object instance handle not found in shipIndexMap" << std::endl;
            return;
        }
    
        //Update otherShipValues for each ship and print out the updated values for otherShip
    
        Ship& enemyShip = enemyShips[itEnemyShip->second];
    
        auto itShipFederateName = theAttributes.find(attributeHandleEnemyShipFederateName);
        if (itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itShipFederateName->second);
            enemyShip.shipName = attributeValueFederateName.get();

            std::wcout << L"Updated target federate name: " << enemyShip.shipName << std::endl;
        } else {
            std::wcerr << L"Attribute handle for ship federate name not found" << std::endl;
        }
    
        auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
        if (itEnemyShipPosition != theAttributes.end()) {
            std::pair<double, double> tempShipPosition = decodePositionRec(itEnemyShipPosition->second);
            enemyShip.shipPosition = tempShipPosition;
            std::wcout << L"Updated target ship position: " << enemyShip.shipPosition.first << L", " << enemyShip.shipPosition.second << L" for the object" << theObject << std::endl;

            std::wcout << L"-------------------------------------------------------------" << std::endl << std::endl;

        } else {
            std::wcerr << L"Attribute handle for ship position not found" << std::endl;
        }
    }


void MyShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Recieve interaction called with time" << std::endl;

    if (interactionClassHandle == interactionClassTargetHit) {

        auto it = parameterValues.find(parameterHandleTargetHitID);
        if (it != parameterValues.end()) {
            rti1516e::VariableLengthData attributeValue = it->second;
            rti1516e::HLAunicodeString value;
            value.decode(attributeValue);
            std::wstring targetID = value.get();
            std::wcout << L"Target ID: " << targetID << std::endl;
            
            for (auto i = friendlyShipIndexMap.begin(); i != friendlyShipIndexMap.end();) {
                Ship& friendlyShip = friendlyShips[i->second];
                if (friendlyShip.shipName == targetID) {
                    friendlyShip.shipHP -= 50; // Assuming 50 is the damage dealt

                    if (friendlyShip.shipHP <= 0) {
                        std::wcout << L"Ship destroyed: " << friendlyShip.shipName << std::endl;

                        // Remove the ship safely
                        size_t index = i->second;
                        if (index != friendlyShips.size() - 1) {
                            friendlyShips[index] = std::move(friendlyShips.back());
                            friendlyShipIndexMap[friendlyShips[index].objectInstanceHandle] = index;
                        }
                        friendlyShips.pop_back();
                        i = friendlyShipIndexMap.erase(i); // Erase and get the next iterator
                        continue; // Skip incrementing the iterator
                    } else {
                        std::wcout << L"Ship hit: " << friendlyShip.shipName << L" HP: " << friendlyShip.shipHP << std::endl;
                    }
                }
                i++;
            }
        }
    }
}

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
        if (federateName == L"BlueShipFederate") {
            std::wcout << L"Creating blue ships" << std::endl;
            createNewShips(paramValueBlueShips.get());
        }

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << std::endl << L": Red ships: " << paramValueRedShips.get() << std::endl;
        std::wcout << federateName << std::endl;
        if (federateName == L"RedShipFederate") {
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
        std::wcout << L"Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipFederate") {
        std::wcout << L"Federate synchronized at BlueTeamSync." << std::endl;
        blueSyncLabel = label;
    }
    if (label == L"RedShipFederate") {
        std::wcout << L"Federate synchronized at RedTeamSync." << std::endl;
        redSyncLabel = label;
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
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

void MyShipFederateAmbassador::createNewShips(int amountOfShips) {
    try {
        for (int i = 0; i < amountOfShips; i++) {
            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(objectClassHandleShip);
            addShip(objectInstanceHandle);
            double latitude = 20.43829000;
            double longitude = 15.62534000;


            if (federateName == L"BlueShipFederate") {
                friendlyShips.back().shipName = L"BlueShip " + std::to_wstring(shipCounter++); 
                friendlyShips.back().shipTeam = L"Blue";
            } 
            else {
                friendlyShips.back().shipName = L"RedShip " + std::to_wstring(shipCounter++); 
                friendlyShips.back().shipTeam = L"Red";
            } 

            friendlyShips.back().shipPosition = generateDoubleShipPosition(latitude, longitude, friendlyShips.back().shipTeam, i, amountOfShips);

            readJsonFile();

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(friendlyShips.back().shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(friendlyShips.back().shipPosition.second));

            std::wcout << L"Registered ship object" << friendlyShips.back().shipName << std::endl;

            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandleShipFederateName] = rti1516e::HLAunicodeString(friendlyShips.back().shipName).encode();
            attributes[attributeHandleShipPosition] = shipPositionRecord.encode();
            attributes[attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[attributeHandleNumberOfMissiles] = rti1516e::HLAinteger32BE(friendlyShips.back().shipNumberOfMissiles).encode();
            //Eventually add numberOfCanons

            //Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
        return;
    }
    createShips = true;
}

void MyShipFederateAmbassador::readJsonFile() {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);

    //randomly select a ship configuration
    if (!parser.isFileOpen()) return;
    
    int i = dis(gen); //Randomly select a ship configuration

    parser.parseShipConfig("Ship" + std::to_string(i));
    friendlyShips.back().shipSize = parser.getShipSize();
    std::wcout << std::endl << L"Ship size: " << friendlyShips.back().shipSize << L" for ship " << friendlyShips.back().shipName << std::endl;
    friendlyShips.back().shipNumberOfMissiles = parser.getNumberOfMissiles();
    std::wcout << L"Number of missiles: " << friendlyShips.back().shipNumberOfMissiles << L" for ship " << friendlyShips.back().shipName << std::endl;
    friendlyShips.back().shipNumberOfCanons = parser.getNumberOfCanons();
    std::wcout << L"Number of canons: " << friendlyShips.back().shipNumberOfCanons << L" for ship " << friendlyShips.back().shipName << std::endl;
    
}

void MyShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    friendlyShips.emplace_back(objectHandle);
    friendlyShipIndexMap[objectHandle] = friendlyShips.size() - 1;
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


