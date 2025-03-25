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
        std::wcout << L": Red ships: " << paramValueRedShips.get() << std::endl;

        rti1516e::HLAfloat64BE paramValueTimeScaleFactor;
        paramValueTimeScaleFactor.decode(itTimeScaleFactor->second);
        std::wcout << L": Time scale factor: " << paramValueTimeScaleFactor.get() << std::endl;
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

            setMyShipPosition(generateDoubleShootShipPosition(latitude, longitude));


            ships.back().shipName = L"ShootShip " + std::to_wstring(shipCounter++); //In case 'new' ships get added mid simulation
            ships.back().shipPosition.first = 20.43829000;
            ships.back().shipPosition.second = 15.62534000;

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ships.back().shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ships.back().shipPosition.second));

            std::wcout << L"Registered ship object" << std::endl;

            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandleShipFederateName] = rti1516e::HLAunicodeString(ships.back().shipName).encode();
            attributes[attributeHandleShipPosition] = shipPositionRecord.encode();
            attributes[attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(getSpeed(10, 10, 25)).encode();
            attributes[attributeHandleNumberOfMissiles] = rti1516e::HLAinteger32BE(numberOfMissiles).encode();

            //Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            _rtiambassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());

        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShootShipFederateAmbassador::addShip(rti1516e::ObjectInstanceHandle objectHandle) {
    ships.emplace_back(objectHandle);
    shipIndexMap[objectHandle] = ships.size() - 1;
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
std::pair<double, double> MyShootShipFederateAmbassador::getMyShipPosition() const {
    return myShipPosition;
}
void MyShootShipFederateAmbassador::setMyShipPosition(const std::pair<double, double>& position) {
    myShipPosition = position;
}

std::wstring MyShootShipFederateAmbassador::getMyShipFederateName() const {
    return myShipFederateName;
}
void MyShootShipFederateAmbassador::setMyShipFederateName(const std::wstring& name) {
    myShipFederateName = name;
}

double MyShootShipFederateAmbassador::getMyShipSpeed() const {
    return myShipSpeed;
}
void MyShootShipFederateAmbassador::setMyShipSpeed(const double& speed) {
    myShipSpeed = speed;
}

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

//Json values get/set
std::wstring MyShootShipFederateAmbassador::getshipNumber() const {
    return shipNumber;
}
void MyShootShipFederateAmbassador::setshipNumber(const std::wstring& name) {
    shipNumber = name;
}

double MyShootShipFederateAmbassador::getshipheight() const {
    return shipheight;
}
void MyShootShipFederateAmbassador::setshipheight(const double& height) {
    shipheight = height;
}

double MyShootShipFederateAmbassador::getshipwidth() const {
    return shipwidth;
}
void MyShootShipFederateAmbassador::setshipwidth(const double& width) {
    shipwidth = width;
}

double MyShootShipFederateAmbassador::getshiplength() const {
    return shiplength;
}
void MyShootShipFederateAmbassador::setshiplength(const double& length) {
    shiplength = length;
}

double MyShootShipFederateAmbassador::getShipSize() {
    return shiplength * shipwidth * shipheight;
}

int MyShootShipFederateAmbassador::getNumberOfMissiles() const {
    return numberOfMissiles;
}
void MyShootShipFederateAmbassador::setNumberOfMissiles(const int& numMissiles) {
    numberOfMissiles = numMissiles;
}