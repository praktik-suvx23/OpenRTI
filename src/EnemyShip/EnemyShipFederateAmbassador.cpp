#include "EnemyShipFederateAmbassador.h"



EnemyShipFederateAmbassador::EnemyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance) 
    : _rtiambassador(rtiAmbassador), instance(instance) {
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
    std::wcout << L"[DEBUG] Reflect attribute values called in object "<< theObject << std::endl;

    //Debugging for attribute values and map
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
        std::wcout << L"-------------------------------------------------------------" << std::endl;
        std::wcout << L"Updated target federate name: " << enemyship.shipName << L" for the object" << theObject << std::endl;
    } else {
        std::wcerr << L"Attribute handle for ship federate name not found" << std::endl;
    }

    auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
    if (itEnemyShipPosition != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueShipPosition;
        attributeValueShipPosition.decode(itEnemyShipPosition->second);
        enemyship.shipPosition = attributeValueShipPosition.get();
        std::wcout << L"Updated target ship position: " << enemyship.shipPosition << L" for the object" << theObject << std::endl;
        std::wcout << L"-------------------------------------------------------------" << std::endl << std::endl;
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
        std::wcout << L"Instance " << instance << L": Blue ships: " << paramValueBlueShips.get() << std::endl;

        rti1516e::HLAinteger32BE paramValueRedShips;
        paramValueRedShips.decode(itRedShips->second);
        std::wcout << L"Instance " << instance << L": Red ships: " << paramValueRedShips.get() << std::endl;
        amountOfShips = paramValueRedShips.get();
        createNewShips(amountOfShips);

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << L"Instance " << instance << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
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

            // Set ship attributes
            ships.back().shipName = L"EnemyShip " + std::to_wstring(i);
            ships.back().numberOfMissiles = i;
            std::wcout << L"Registered ship object " << ships.back().shipName << std::endl;

            // Prepare attribute values
            rti1516e::AttributeHandleValueMap attributes;
            attributes[getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(ships.back().shipName).encode();
            attributes[getAttributeHandleMyShipPosition()] = rti1516e::HLAunicodeString(L"20.43829000,1562534000").encode();
            attributes[getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(speedDis(gen)).encode();
            attributes[getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(ships.back().numberOfMissiles).encode();

            // Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
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

//Json values get/set
std::wstring EnemyShipFederateAmbassador::getshipNumber() const {
    return shipNumber;
}
void EnemyShipFederateAmbassador::setshipNumber(const std::wstring& name) {
    shipNumber = name;
}

double EnemyShipFederateAmbassador::getshipheight() const {
    return shipheight;
}
void EnemyShipFederateAmbassador::setshipheight(const double& height) {
    shipheight = height;
}

double EnemyShipFederateAmbassador::getshipwidth() const {
    return shipwidth;
}
void EnemyShipFederateAmbassador::setshipwidth(const double& width) {
    shipwidth = width;
}

double EnemyShipFederateAmbassador::getshiplength() const {
    return shiplength;
}
void EnemyShipFederateAmbassador::setshiplength(const double& length) {
    shiplength = length;
}

double EnemyShipFederateAmbassador::getShipSize() {
    ShipSize = shiplength * shipwidth * shipheight;
    return ShipSize;
}

void EnemyShipFederateAmbassador::setNumberOfRobots(const int& numRobots) {
    numberOfRobots = numRobots;
}
int EnemyShipFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}