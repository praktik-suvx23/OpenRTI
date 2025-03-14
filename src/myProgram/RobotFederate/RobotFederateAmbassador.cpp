#include "RobotFederateAmbassador.h"


MyFederateAmbassador::MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance)
    : _rtiAmbassador(rtiAmbassador), instance(instance), _expectedShipName(TargetFederate) {
}

MyFederateAmbassador::~MyFederateAmbassador() {}



void MyFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"Synchronization point announced: " << label << std::endl;
}

void MyFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void MyFederateAmbassador::reflectAttributeValues(
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

void MyFederateAmbassador::receiveInteraction(
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
        
        auto itFireParamHandle = parameterValues.find(fireParamHandle);
        auto itTargetParamHandle = parameterValues.find(targetParamHandle);
        auto itShootingShipPosition = parameterValues.find(startPosRobot);
        auto itTargetShipPosition = parameterValues.find(targetShipPosition);

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
    }
}

void MyFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MyFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MyFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) { //Used for time management
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

// Getters and setters for the attributes handles
rti1516e::ObjectClassHandle MyFederateAmbassador::getMyObjectClassHandle() const {
    return shipClassHandle;
}

void MyFederateAmbassador::setMyObjectClassHandle(const rti1516e::ObjectClassHandle& handle) {
    shipClassHandle = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleShipTag() const {
    return attributeHandleShipTag;
}
void MyFederateAmbassador::setAttributeHandleShipTag(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTag = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MyFederateAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleFutureShipPosition() const {
    return attributeHandleFutureShipPosition;
}
void MyFederateAmbassador::setAttributeHandleFutureShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleFutureShipPosition = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MyFederateAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleShipSize() const {
    return attributeHandleShipSize;
}
void MyFederateAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSize = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleNumberOfRobots() const {
    return attributeHandleNumberOfRobots;
}
void MyFederateAmbassador::setAttributeHandleNumberOfRobots(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfRobots = handle;
}

rti1516e::AttributeHandle MyFederateAmbassador::getAttributeHandleFederateName() const {
    return attributeHandleShipFederateName;
}
void MyFederateAmbassador::setAttributeHandleFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
}

//getters and setters for attributes
double MyFederateAmbassador::getCurrentAltitude() const {
    return currentAltitude;
}
void MyFederateAmbassador::setCurrentAltitude(double altitude) {
    currentAltitude = altitude;
}

double MyFederateAmbassador::getCurrentSpeed() const {
    return currentSpeed;
}
void MyFederateAmbassador::setCurrentSpeed(const double& speed) {
    currentSpeed = speed;
}

double MyFederateAmbassador::getCurrentFuelLevel() const {
    return currentFuelLevel;
}
void MyFederateAmbassador::setCurrentFuelLevel(const double& fuelLevel) {
    currentFuelLevel = fuelLevel;
}

std::wstring MyFederateAmbassador::getCurrentPosition() const {
    return currentPosition;
}
void MyFederateAmbassador::setCurrentPosition(const std::wstring& position) {
    currentPosition = position;
}
std::wstring MyFederateAmbassador::getShipPosition() const {
    return shipPosition;
}
void MyFederateAmbassador::setShipPosition(const std::wstring& position) {
    shipPosition = position;
}

double MyFederateAmbassador::getCurrentDistance() const {
    return currentDistance;
}
void MyFederateAmbassador::setCurrentDistance(const double& distance) {
    currentDistance = distance;
}
int MyFederateAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MyFederateAmbassador::setNumberOfRobots(const int& robots) {
    numberOfRobots = robots;
}
// general get and set functions
std::wstring MyFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MyFederateAmbassador::getFederateName() const {
    return federateName;
}
void MyFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

//get and set for fire interaction
rti1516e::InteractionClassHandle MyFederateAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void MyFederateAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle MyFederateAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void MyFederateAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

rti1516e::ParameterHandle MyFederateAmbassador::getTargetParam() const {
    return targetParamHandle;
}
void MyFederateAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {
    targetParamHandle = handle;
}

rti1516e::ParameterHandle MyFederateAmbassador::getStartPosRobot() const {
    return startPosRobot;
}
void MyFederateAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {
    startPosRobot = handle;
}

// Interactions that are for the moment not implemented
bool MyFederateAmbassador::getAssignedTarget() const {
    return assignedTarget;
}

bool MyFederateAmbassador::getHitStatus() const {
    return hitStatus;
}

std::wstring MyFederateAmbassador::getTargetShipID() const {
    return _targetShipID;
}
// is this in use???
std::wstring MyFederateAmbassador::getShipID() const {
    return shipID;
}

int MyFederateAmbassador::getDamageAmount() const {
    return damageAmount;
}