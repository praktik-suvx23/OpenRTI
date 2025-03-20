#include "EnemyShipFederateAmbassador.h"



EnemyShipFederateAmbassador::EnemyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance) 
    : _rtiambassador(rtiAmbassador), instance(instance) {
    _expectedShipName = L"ShootShipFederate " + std::to_wstring(instance);
}

EnemyShipFederateAmbassador::~EnemyShipFederateAmbassador() {}

void EnemyShipFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
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

        if (_shipInstances.find(theObject) != _shipInstances.end()) {
            std::wcout << L"Object " << theObject << L" is present in _shipInstances, ignoring." << std::endl;
            return;
        }

        //Debugging for attribute values and map
        for (auto it = theAttributes.begin(); it != theAttributes.end(); ++it) {
            std::wcout << L"Attribute handle: " << it->first << std::endl;
        }
        for (auto it = shipIndexMap.begin(); it != shipIndexMap.end(); ++it) {
            std::wcout << L"ShipIndexMap: " << it->first << std::endl;
            std::wcout << L"ShipIndexMap: " << it->second << std::endl;
        }
        auto itShip = shipIndexMap.find(theObject);
        std::wcout << L"Shipmap first value" << itShip->first << std::endl;
        std::wcout << L"Shipmap second value" << itShip->second << std::endl;

        if (itShip == shipIndexMap.end()) {
            std::wcerr << L"Object instance handle not found in shipIndexMap" << std::endl;
            return;
        }

        Ship& ship = ships[itShip->second];

        auto itShipFederateName = theAttributes.find(attributeHandleMyShipFederateName);
        if (itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itShipFederateName->second);
            ship.shipName = attributeValueFederateName.get();
            std::wcout << L"Updated federate name: " << ship.shipName << std::endl;
        } else {
            std::wcerr << L"Attribute handle for ship federate name not found" << std::endl;
        }

        auto itShipPosition = theAttributes.find(attributeHandleMyShipPosition);
        if (itShipPosition != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueShipPosition;
            attributeValueShipPosition.decode(itShipPosition->second);
            ship.shipPosition = attributeValueShipPosition.get();
            std::wcout << L"Updated ship position: " << ship.shipPosition << std::endl;
        } else {
            std::wcerr << L"Attribute handle for ship position not found" << std::endl;
        }

        auto itShipSpeed = theAttributes.find(attributeHandleMyShipSpeed);
        if (itShipSpeed != theAttributes.end()) {
            rti1516e::HLAfloat64BE attributeValueShipSpeed;
            attributeValueShipSpeed.decode(itShipSpeed->second);
            ship.shipSpeed = attributeValueShipSpeed.get();
            std::wcout << L"Updated ship speed: " << ship.shipSpeed << std::endl;
        } else {
            std::wcerr << L"Attribute handle for ship speed not found" << std::endl;
        }

        auto itNumberOfMissiles = theAttributes.find(attributeHandleNumberOfMissiles);
        if (itNumberOfMissiles != theAttributes.end()) {
            rti1516e::HLAinteger32BE attributeValueNumberOfMissiles;
            attributeValueNumberOfMissiles.decode(itNumberOfMissiles->second);
            ship.numberOfMissiles = attributeValueNumberOfMissiles.get();
            std::wcout << L"Updated number of missiles: " << ship.numberOfMissiles << std::endl;
        } else {
            std::wcerr << L"Attribute handle for number of missiles not found" << std::endl;
        }
    /*
    auto itShipFederateName = theAttributes.find(attributeHandleMyShipFederateName);
    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        if (attributeValueFederateName.get() != _expectedShipName) {
            std::wcout << L"Instance " << instance << L": Unexpected ship name: " << attributeValueFederateName.get() << std::endl;
            return;
        } else {
            std::wcout << L"Instance " << instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
        }
    }

    //Calculate distance between ships
    auto itEnemyShipPosition = theAttributes.find(attributeHandleEnemyShipPosition);
    auto itEnemyShipFederateName = theAttributes.find(attributeHandleEnemyShipFederateName);

    if (itEnemyShipPosition != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueEnemyShipPosition;
        attributeValueEnemyShipPosition.decode(itEnemyShipPosition->second);
        setEnemyShipPosition(attributeValueEnemyShipPosition.get());
    }
    if (itEnemyShipFederateName != theAttributes.end()) { //Use this to start a robot to shoot at this federateName 
        rti1516e::HLAunicodeString attributeValueEnemyShipFederateName;
        attributeValueEnemyShipFederateName.decode(itEnemyShipFederateName->second);
        setEnemyShipFederateName(attributeValueEnemyShipFederateName.get());
    } */
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
            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiambassador->registerObjectInstance(getMyObjectClassHandle());
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

            // Update attribute values
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

void EnemyShipFederateAmbassador::setShipPosition(rti1516e::ObjectInstanceHandle objectHandle, const std::wstring& position) {
    auto it = shipIndexMap.find(objectHandle);
    if (it != shipIndexMap.end()) {
        ships[it->second].shipPosition = position;
    } 
    else {
        std::wcerr << L"Object instance handle not found in shipIndexMap" << std::endl;
    }
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
int EnemyShipFederateAmbassador::getAmountOfShips() const {
    return amountOfShips;
}
void EnemyShipFederateAmbassador::setAmountOfShips(const int& ships) {
    amountOfShips = ships;
}

double EnemyShipFederateAmbassador::getTimeScale() const {
    return timeScale;
}
void EnemyShipFederateAmbassador::setTimeScale(const double& scale) {
    timeScale = scale;
}

// Getters and setters for ship attributes
std::wstring EnemyShipFederateAmbassador::getMyShipPosition() const {
    return myShipPosition;
}
void EnemyShipFederateAmbassador::setMyShipPosition(const std::wstring& position) {
    myShipPosition = position;
}

std::wstring EnemyShipFederateAmbassador::getMyShipFederateName() const {
    return myShipFederateName;
}
void EnemyShipFederateAmbassador::setMyShipFederateName(const std::wstring& name) {
    myShipFederateName = name;
}

double EnemyShipFederateAmbassador::getMyShipSpeed() const {
    return myShipSpeed;
}
void EnemyShipFederateAmbassador::setMyShipSpeed(const double& speed) {
    myShipSpeed = speed;
}

std::wstring EnemyShipFederateAmbassador::getEnemyShipFederateName() const {
    return enemyShipFederateName;
}
void EnemyShipFederateAmbassador::setEnemyShipFederateName(const std::wstring& name) {
    enemyShipFederateName = name;
}

std::wstring EnemyShipFederateAmbassador::getEnemyShipPosition() const {
    return enemyShipPosition;
}
void EnemyShipFederateAmbassador::setEnemyShipPosition(const std::wstring& position) {
    enemyShipPosition = position;
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