#include "ShootShipFederateAmbassador.h"

MyShootShipFederateAmbassador::MyShootShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiambassador(rtiAmbassador)  {
}

MyShootShipFederateAmbassador::~MyShootShipFederateAmbassador() {}

void MyShootShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;

    enemyShips.emplace_back(theObject);
    enemyShipIndexMap[theObject] = enemyShips.size() - 1;
    std::wcout << L"Enemy ship instance handle: " << theObject << std::endl;
}

void MyShootShipFederateAmbassador::reflectAttributeValues(
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
    
        EnemyShip& enemyship = enemyShips[itEnemyShip->second];
    
        auto itShipFederateName = theAttributes.find(attributeHandleEnemyShipFederateName);
        if (itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itShipFederateName->second);
            enemyship.shipName = attributeValueFederateName.get();

            std::wcout << L"Updated target federate name: " << enemyship.shipName << std::endl;
        } else {
            std::wcerr << L"Attribute handle for ship federate name not found" << std::endl;
        }
    
        auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
        if (itEnemyShipPosition != theAttributes.end()) {
            std::pair<double, double> tempShipPosition = decodePositionRec(itEnemyShipPosition->second);
            enemyship.shipPosition = tempShipPosition;
            std::wcout << L"Updated target ship position: " << enemyship.shipPosition.first << L", " << enemyship.shipPosition.second << L" for the object" << theObject << std::endl;

            std::wcout << L"-------------------------------------------------------------" << std::endl << std::endl;

            //Temporary solution to get target position
            setEnemyShipPosition(enemyship.shipPosition);
        } else {
            std::wcerr << L"Attribute handle for ship position not found" << std::endl;
        }
    }


void MyShootShipFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
    std::wcout << L"[DEBUG] Recieve interaction called with time" << std::endl;
}

void MyShootShipFederateAmbassador::receiveInteraction(
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
        createNewShips(paramValueBlueShips.get());

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << std::endl << L": Red ships: " << paramValueRedShips.get() << std::endl;

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << std::endl << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
        timeScale = paramValueTimeScaleFactor.get();
    }
}



void MyShootShipFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) 
{
    if (label == L"InitialSync") {
        std::wcout << L"Shooter Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"Master Federate synchronized at SimulationSetupComplete." << std::endl;
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

void MyShootShipFederateAmbassador::createNewShips(int amountOfShips) {
    try {
        for (int i = 0; i < amountOfShips; i++) {
            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(objectClassHandleShip);
            addShip(objectInstanceHandle);
            double latitude = 20.43829000;
            double longitude = 15.62534000;


            friendlyShips.back().shipName = L"ShootShip " + std::to_wstring(shipCounter++); //In case 'new' ships get added mid simulation shipcounter++
            friendlyShips.back().shipTeam = L"Blue";
            friendlyShips.back().shipPosition = generateDoubleShootShipPosition(latitude, longitude);

            readJsonFile();

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(friendlyShips.back().shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(friendlyShips.back().shipPosition.second));

            std::wcout << L"Registered ship object" << friendlyShips.back().shipName << std::endl;

            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandleShipFederateName] = rti1516e::HLAunicodeString(friendlyShips.back().shipName).encode();
            attributes[attributeHandleShipPosition] = shipPositionRecord.encode();
            attributes[attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[attributeHandleNumberOfMissiles] = rti1516e::HLAinteger32BE(friendlyShips.back().numberOfMissiles).encode();
            //Eventually add numberOfCanons

            //Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());

        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShootShipFederateAmbassador::readJsonFile() {
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
    friendlyShips.back().numberOfMissiles = parser.getNumberOfMissiles();
    std::wcout << L"Number of missiles: " << friendlyShips.back().numberOfMissiles << L" for ship " << friendlyShips.back().shipName << std::endl;
    friendlyShips.back().numberOfCanons = parser.getNumberOfCanons();
    std::wcout << L"Number of canons: " << friendlyShips.back().numberOfCanons << L" for ship " << friendlyShips.back().shipName << std::endl;
    
}

void MyShootShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    friendlyShips.emplace_back(objectHandle);
    friendlyShipIndexMap[objectHandle] = friendlyShips.size() - 1;
}

// Getter and setter for Object Class Ship and its attributes
rti1516e::ObjectClassHandle MyShootShipFederateAmbassador::getObjectClassHandleShip() const {
    return objectClassHandleShip;
}
void MyShootShipFederateAmbassador::setObjectClassHandleShip(rti1516e::ObjectClassHandle handle) {
    objectClassHandleShip = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipFederateName() const {
    return attributeHandleShipFederateName;
}
void MyShootShipFederateAmbassador::setAttributeHandleShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void MyShootShipFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MyShootShipFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MyShootShipFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MyShootShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void MyShootShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
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

// Getter and setter for interaction class SetupSimulation and its parameters
rti1516e::InteractionClassHandle MyShootShipFederateAmbassador::getSetupSimulationHandle() const {
    return setupSimulationHandle;
}
void MyShootShipFederateAmbassador::setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle) {
    setupSimulationHandle = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getBlueShipsParam() const {
    return blueShips;
}
void MyShootShipFederateAmbassador::setBlueShipsParam(const rti1516e::ParameterHandle& handle) {
    blueShips = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getRedShipsParam() const {
    return redShips;
}
void MyShootShipFederateAmbassador::setRedShipsParam(const rti1516e::ParameterHandle& handle) {
    redShips = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getTimeScaleFactorParam() const {
    return timeScaleFactor;
}
void MyShootShipFederateAmbassador::setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle) {
    timeScaleFactor = handle;
}

// Getters and setters for ship attributes

std::wstring MyShootShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void MyShootShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::pair<double, double> MyShootShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void MyShootShipFederateAmbassador::setEnemyShipPosition(const std::pair<double, double>& position) {
    enemyShipPosition = position;
}

double MyShootShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void MyShootShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double MyShootShipFederateAmbassador::getBearing() const {
    return bearing;
}
void MyShootShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool MyShootShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void MyShootShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring MyShootShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

// Getter and setter functions for interaction class FireMissile
rti1516e::InteractionClassHandle MyShootShipFederateAmbassador::getInteractionClassFireMissile() const {
    return interactionClassFireMissile;
}
void MyShootShipFederateAmbassador::setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle) {
    interactionClassFireMissile = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}
void MyShootShipFederateAmbassador::setParamShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleShooterID = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}
void MyShootShipFederateAmbassador::setParamMissileTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTeam = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}
void MyShootShipFederateAmbassador::setParamMissileStartPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileStartPosition = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}
void MyShootShipFederateAmbassador::setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTargetPosition = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MyShootShipFederateAmbassador::setParamNumberOfMissilesFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleNumberOfMissilesFired = handle;
}

rti1516e::ParameterHandle MyShootShipFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
void MyShootShipFederateAmbassador::setParamMissileSpeed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileSpeed = handle;
}
