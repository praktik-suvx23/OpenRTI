#include "EnemyShipFederateAmbassador.h"



EnemyShipFederateAmbassador::EnemyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) 
    : _rtiambassador(rtiAmbassador) {
}

EnemyShipFederateAmbassador::~EnemyShipFederateAmbassador() {}

void EnemyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;

    enemyShips.emplace_back(theObject);
    enemyShipIndexMap[theObject] = enemyShips.size() - 1;
    std::wcout << L"Enemy ship instance handle: " << theObject << std::endl;
}

void EnemyShipFederateAmbassador::reflectAttributeValues(
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


void EnemyShipFederateAmbassador::receiveInteraction(
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

void EnemyShipFederateAmbassador::receiveInteraction(
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

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << L": Red ships: " << paramValueRedShips.get() << std::endl;
        createNewShips(paramValueRedShips.get());

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << std::endl << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
        setTimeScale(paramValueTimeScaleFactor.get());
    }
}

void EnemyShipFederateAmbassador::announceSynchronizationPoint(
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

void EnemyShipFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void EnemyShipFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void EnemyShipFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime& theTime) {
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

void EnemyShipFederateAmbassador::createNewShips(int amountOfShips) {
    try {
        for (int i = 0; i < amountOfShips; i++) {
            // Register the object instance
            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(objectClassHandle);
            addShip(objectInstanceHandle);
            double latitude = 20.43829000;
            double longitude = 15.62534000;


            // Set ship attributes
            ships.back().shipName = L"EnemyShip " + std::to_wstring(shipCounter++); //In case 'new' ships get added mid simulation shipcounter++
            ships.back().shipPosition = generateDoubleShipPosition(latitude, longitude);

            readJsonFile();

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ships.back().shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ships.back().shipPosition.second));

            std::wcout << L"Registered ship object " << ships.back().shipName << std::endl;

            // Prepare attribute values
            rti1516e::AttributeHandleValueMap attributes;
            attributes[getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(ships.back().shipName).encode();
            attributes[getAttributeHandleMyShipPosition()] = shipPositionRecord.encode();
            attributes[getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(ships.back().numberOfMissiles).encode();

            // Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederateAmbassador::readJsonFile() {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);

    //randomly select a ship configuration
    if (!parser.isFileOpen()) return;
    
    int i = dis(gen); //Randomly select a ship configuration

    parser.parseShipConfig("Ship" + std::to_string(i));
    ships.back().shipSize = parser.getShipSize();
    std::wcout << std::endl << L"Ship size: " << ships.back().shipSize << L" for ship " << ships.back().shipName << std::endl;
    ships.back().numberOfMissiles = parser.getNumberOfMissiles();
    std::wcout << L"Number of missiles: " << ships.back().numberOfMissiles << L" for ship " << ships.back().shipName << std::endl;
    ships.back().numberOfCanons = parser.getNumberOfCanons();
    std::wcout << L"Number of canons: " << ships.back().numberOfCanons << L" for ship " << ships.back().shipName << std::endl;
    
}

void EnemyShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    ships.emplace_back(objectHandle);
    shipIndexMap[objectHandle] = ships.size() - 1;
}

// Setters and getters for attribute handles
rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipPosition() const {
    return attributeHandleMyShipPosition;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipPosition = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipFederateName() const {
    return attributeHandleMyShipFederateName;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipFederateName = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleMyShipSpeed() const {
    return attributeHandleMyShipSpeed;
}
void EnemyShipFederateAmbassador::setAttributeHandleMyShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleMyShipSpeed = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void EnemyShipFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleEnemyShipFederateName() const {
    return attributeHandleEnemyShipFederateName;
}
void EnemyShipFederateAmbassador::setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipFederateName = handle;
}

rti1516e::AttributeHandle EnemyShipFederateAmbassador::getAttributeHandleEnemyShipPosition() const {
    return attributeHandleEnemyShipPosition;
}
void EnemyShipFederateAmbassador::setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleEnemyShipPosition = handle;
}

rti1516e::ObjectClassHandle EnemyShipFederateAmbassador::getMyObjectClassHandle() const {
    return objectClassHandle;
}
void EnemyShipFederateAmbassador::setMyObjectClassHandle(rti1516e::ObjectClassHandle handle) {
    objectClassHandle = handle;
}

//Get and set for fire interaction
rti1516e::InteractionClassHandle EnemyShipFederateAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void EnemyShipFederateAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void EnemyShipFederateAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getTargetParam() const {
    return TargetParam;
}
void EnemyShipFederateAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {
    TargetParam = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getStartPosRobot() const {
    return startPosRobot;
}
void EnemyShipFederateAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {
    startPosRobot = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getTargetPositionParam() const {
    return targetPosition;
}
void EnemyShipFederateAmbassador::setTargetPositionParam(const rti1516e::ParameterHandle& handle) {
    targetPosition = handle;
}

//get and set for setup simulation interaction
rti1516e::InteractionClassHandle EnemyShipFederateAmbassador::getSetupSimulationHandle() const {
    return setupSimulationHandle;
}
void EnemyShipFederateAmbassador::setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle) {
    setupSimulationHandle = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getBlueShipsParam() const {
    return blueShips;
}
void EnemyShipFederateAmbassador::setBlueShipsParam(const rti1516e::ParameterHandle& handle) {
    blueShips = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getRedShipsParam() const {
    return redShips;
}
void EnemyShipFederateAmbassador::setRedShipsParam(const rti1516e::ParameterHandle& handle) {
    redShips = handle;
}

rti1516e::ParameterHandle EnemyShipFederateAmbassador::getTimeScaleFactorParam() const {
    return timeScaleFactor;
}
void EnemyShipFederateAmbassador::setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle) {
    timeScaleFactor = handle;
}

//Setup get/set

double EnemyShipFederateAmbassador::getTimeScale() const {
    return timeScale;
}
void EnemyShipFederateAmbassador::setTimeScale(const double& scale) {
    timeScale = scale;
}

// Getters and setters for ship attributes

double EnemyShipFederateAmbassador::getMyShipSpeed() const {
    return myShipSpeed;
}
void EnemyShipFederateAmbassador::setMyShipSpeed(const double& speed) {
    myShipSpeed = speed;
}

double EnemyShipFederateAmbassador::getDistanceBetweenShips() const {
    return distanceBetweenShips;
}
void EnemyShipFederateAmbassador::setDistanceBetweenShips(const double& distance) {
    distanceBetweenShips = distance;
}

double EnemyShipFederateAmbassador::getBearing() const {
    return bearing;
}
void EnemyShipFederateAmbassador::setBearing(const double& input) {
    bearing = input;
}

bool EnemyShipFederateAmbassador::getIsFiring() const {
    return isFiring;
}
void EnemyShipFederateAmbassador::setIsFiring(const bool& firing) {
    isFiring = firing;
}

std::wstring EnemyShipFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

//Remove these eventually

std::pair<double, double> EnemyShipFederateAmbassador::getMyShipPosition() const {
    return myShipPosition;
}
void EnemyShipFederateAmbassador::setMyShipPosition(const std::pair<double, double>& position) {
    myShipPosition = position;
}

void EnemyShipFederateAmbassador::setMyShipFederateName(const std::wstring& name) {
    myShipFederateName = name;
}
std::wstring EnemyShipFederateAmbassador::getMyShipFederateName() const {
    return myShipFederateName;
}

void EnemyShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    _expectedShipName = name;
}
std::wstring EnemyShipFederateAmbassador::getEnemyShipFederateName() const {
    return _expectedShipName;
}

std::pair<double, double> EnemyShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void EnemyShipFederateAmbassador::setEnemyShipPosition(const std::pair<double, double>& position) {
    enemyShipPosition = position;
}