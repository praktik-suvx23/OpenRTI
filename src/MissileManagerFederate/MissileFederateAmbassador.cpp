#include "MissileFederateAmbassador.h"


MissileFederateAmbassador::MissileFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance)
    : _rtiAmbassador(rtiAmbassador), instance(instance), _expectedShipName(TargetFederate) {
}

MissileFederateAmbassador::~MissileFederateAmbassador() {}



void MissileFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"Synchronization point announced: " << label << std::endl;
}

void MissileFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void MissileFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
    std::wstring tempShipID;

    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        
        if (attributeValueFederateName.get() != TargetFederate) {
            return;
        } else {
            std::wcout << L"Instance " << instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
        }
        if (_shipInstances.find(theObject) != _shipInstances.end()) {
            auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
            auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
            auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
            auto itShipSize = theAttributes.find(attributeHandleShipSize);
            auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfRobots);
    
            if (itShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipPosition;
                attributeValueShipPosition.decode(itShipPosition->second);
                shipPosition = attributeValueShipPosition.get();
            }
            if (itFutureShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueFutureShipPosition;
                //attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
                //expectedShipPosition = attributeValueFutureShipPosition.get();
            }
            if (itShipSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSpeed;
                //attributeValueShipSpeed.decode(itShipSpeed->second);
            }
            if (itShipSize != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSize;
                attributeValueShipSize.decode(itShipSize->second);
                shipSize = attributeValueShipSize.get();
                
            }
            if (itNumberOfRobots != theAttributes.end()) {
                rti1516e::HLAinteger32BE attributeValueNumberOfRobots;
                attributeValueNumberOfRobots.decode(itNumberOfRobots->second);
                setNumberOfRobots(attributeValueNumberOfRobots.get());
            }
        }
    }
}

void MissileFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    std::wcout << L"[DEBUG] Recieved fire interaction" << std::endl;
    if (interactionClassHandle == fireRobotHandle) {
        
        try {
            auto itFireParamHandle = parameterValues.find(fireParamHandle);
            auto itTargetParamHandle = parameterValues.find(targetParamHandle);
            auto itShootingShipPosition = parameterValues.find(startPosRobot);
            auto itTargetShipPosition = parameterValues.find(targetPosition);

            if (itFireParamHandle == parameterValues.end() 
            || itTargetParamHandle == parameterValues.end() 
            || itShootingShipPosition == parameterValues.end() 
            || itTargetShipPosition == parameterValues.end()) {
                std::wcerr << L"Missing parameters in fire interaction" << std::endl;
                return;
            }

            rti1516e::HLAinteger32BE paramValueFire;
            paramValueFire.decode(itFireParamHandle->second);
            std::wcout << L"Instance " << instance << L": Fire robot: " << paramValueFire.get() << std::endl;

            rti1516e::HLAunicodeString paramValueTargetShip;
            paramValueTargetShip.decode(itTargetParamHandle->second);
            std::wcout << L"Instance " << instance << L": Target ship: " << paramValueTargetShip.get() << std::endl;

            rti1516e::HLAunicodeString paramValueShootingShipPosition;
            paramValueShootingShipPosition.decode(itShootingShipPosition->second);
            std::wcout << L"Instance " << instance << L": Robot start position: " << paramValueShootingShipPosition.get() << std::endl;

            rti1516e::HLAunicodeString paramValueTargetShipPosition;
            paramValueTargetShipPosition.decode(itTargetShipPosition->second);
            std::wcout << L"Instance " << instance << L": Target ship position: " << paramValueTargetShipPosition.get() << std::endl;

            setShipPosition(paramValueTargetShipPosition.get());
            setCurrentPosition(paramValueShootingShipPosition.get());
            TargetFederate = paramValueTargetShip.get();
            
            startFire = true;

            for (int i = 0; i < paramValueFire.get(); i++) {
                std::wcout << L"Instance " << instance << L": Robot " << i << L" is firing" << std::endl;
                //Implement Logic to create RobotFederates equal to ValueFire also implement numberOfRobots logic
            }
        } catch (const rti1516e::Exception& e) {
            std::wcerr << L"Exception: " << e.what() << std::endl;
        }
        
    }
}

void MissileFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MissileFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MissileFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) { //Used for time management
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

// Getters and setters Object Class Ship and its attributes
rti1516e::ObjectClassHandle MissileFederateAmbassador::getObjectClassHandleShip() const {
    return objectClassHandleShip;
}
void MissileFederateAmbassador::setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle) {
    objectClassHandleShip = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleFederateName() const {
    return attributeHandleShipFederateName;
}
void MissileFederateAmbassador::setAttributeHandleFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleShipTeam() const {
    return attributeHandleShipTeam;
}
void MissileFederateAmbassador::setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTeam = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MissileFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleFutureShipPosition() const {
    return attributeHandleFutureShipPosition;
}
void MissileFederateAmbassador::setAttributeHandleFutureShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleFutureShipPosition = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MissileFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleShipSize() const {
    return attributeHandleShipSize;
}
void MissileFederateAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSize = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleNumberOfMissiles() const {
    return attributeHandleNumberOfMissiles;
}
void MissileFederateAmbassador::setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfMissiles = handle;
}

// Getters and setters Object Class Missile and its attributes
rti1516e::ObjectClassHandle MissileFederateAmbassador::getObjectClassHandleMissile() const {
    return objectClassHandleMissile;
}
void MissileFederateAmbassador::setObjectClassHandleMissile(const rti1516e::ObjectClassHandle& handle) {
    objectClassHandleMissile = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleMissileTeam() const {
    return attributeHandleMissileTeam;
}
void MissileFederateAmbassador::setAttributeHandleMissileTeam(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileTeam = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleMissilePosition() const {
    return attributeHandleMissilePosition;
}
void MissileFederateAmbassador::setAttributeHandleMissilePosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissilePosition = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleMissileAltitude() const {
    return attributeHandleMissileAltitude;
}
void MissileFederateAmbassador::setAttributeHandleMissileAltitude(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileAltitude = handle;
}

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleMissileSpeed() const {
    return attributeHandleMissileSpeed;
}
void MissileFederateAmbassador::setAttributeHandleMissileSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileSpeed = handle;
}

//get and set for fire interaction
rti1516e::InteractionClassHandle MissileFederateAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void MissileFederateAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void MissileFederateAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getTargetParam() const {
    return targetParamHandle;
}
void MissileFederateAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {
    targetParamHandle = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getStartPosRobot() const {
    return startPosRobot;
}
void MissileFederateAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {
    startPosRobot = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getTargetPositionParam() const {
    return targetPosition;
}
void MissileFederateAmbassador::setTargetPositionParam(const rti1516e::ParameterHandle& handle) {
    targetPosition = handle;
}

// Getter and setter functions for interaction class SetupSimulation
rti1516e::InteractionClassHandle MissileFederateAmbassador::getInteractionClassSetupSimulation() const {
    return interactionClassSetupSimulation;
}
void MissileFederateAmbassador::setInteractionClassSetupSimulation(const rti1516e::InteractionClassHandle& handle) {
    interactionClassSetupSimulation = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamTimeScaleFactor() const {
    return parameterHandleSimulationTime;
}
void MissileFederateAmbassador::setParamTimeScaleFactor(const rti1516e::ParameterHandle& handle) {
    parameterHandleSimulationTime = handle;
}

// Getter and setter functions for interaction class FireMissile
rti1516e::InteractionClassHandle MissileFederateAmbassador::getInteractionClassMissile() const {
    return interactionClassFireMissile;
}
void MissileFederateAmbassador::setInteractionClassMissile(const rti1516e::InteractionClassHandle& handle) {
    interactionClassFireMissile = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}
void MissileFederateAmbassador::setParamMissileTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTeam = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissilePosition;
}
void MissileFederateAmbassador::setParamMissileStartPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissilePosition = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTarget;
}
void MissileFederateAmbassador::setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTarget = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamNumberOfMissilesFired() const {
    return parameterHandleNumberOfMissilesFired;
}
void MissileFederateAmbassador::setParamNumberOfMissilesFired(const rti1516e::ParameterHandle& handle) {
    parameterHandleNumberOfMissilesFired = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileSpeed() const {
    return parameterHandleMissileSpeed;
}
void MissileFederateAmbassador::setParamMissileSpeed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileSpeed = handle;
}

//getters and setters for attributes
double MissileFederateAmbassador::getCurrentAltitude() const {
    return currentAltitude;
}
void MissileFederateAmbassador::setCurrentAltitude(double altitude) {
    currentAltitude = altitude;
}

double MissileFederateAmbassador::getCurrentSpeed() const {
    return currentSpeed;
}
void MissileFederateAmbassador::setCurrentSpeed(const double& speed) {
    currentSpeed = speed;
}

double MissileFederateAmbassador::getCurrentFuelLevel() const {
    return currentFuelLevel;
}
void MissileFederateAmbassador::setCurrentFuelLevel(const double& fuelLevel) {
    currentFuelLevel = fuelLevel;
}

std::wstring MissileFederateAmbassador::getCurrentPosition() const {
    return currentPosition;
}
void MissileFederateAmbassador::setCurrentPosition(const std::wstring& position) {
    currentPosition = position;
}
std::wstring MissileFederateAmbassador::getShipPosition() const {
    return shipPosition;
}
void MissileFederateAmbassador::setShipPosition(const std::wstring& position) {
    shipPosition = position;
}

double MissileFederateAmbassador::getCurrentDistance() const {
    return currentDistance;
}
void MissileFederateAmbassador::setCurrentDistance(const double& distance) {
    currentDistance = distance;
}
int MissileFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MissileFederateAmbassador::setNumberOfRobots(const int& robots) {
    numberOfRobots = robots;
}
// general get and set functions
std::wstring MissileFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MissileFederateAmbassador::getFederateName() const {
    return federateName;
}
void MissileFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

