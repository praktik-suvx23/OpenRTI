#include "MissileManagerFederateAmbassador.h"
#include "../include/decodePosition.h"
#include "include/MissileManagerHelper.h"

MissileManagerAmbassador::MissileManagerAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MissileManagerAmbassador::~MissileManagerAmbassador() {}

void MissileManagerAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"Synchronization point announced: " << label << std::endl;
}

void MissileManagerAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

void MissileManagerAmbassador::reflectAttributeValues(
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

void MissileManagerAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    std::wcout << L"[DEBUG] receivedInteraction: " << interactionClassHandle << std::endl;
    
    if (fireMissileHandle == interactionClassHandle) {
        std::wcout << L"[INFO] FireMissile interaction received" << std::endl;

        bool hasShooterID = false, hasTargetID = false, hasShooterPosition = false, hasTargetPosition = false;

        for (const auto& param : parameterValues) {
            if (param.first == shooterIDParamHandle) {
                rti1516e::HLAunicodeString hlaShooterID;
                hlaShooterID.decode(param.second);
                MissileManagerSetter::setShooterID(*this, hlaShooterID.get());
                hasShooterID = true;
            }
            else if (param.first == targetIDParamHandle) {
                rti1516e::HLAunicodeString hlaTargetID;
                hlaTargetID.decode(param.second);
                MissileManagerSetter::setTargetID(*this, hlaTargetID.get());
                hasTargetID = true;
            }
            else if (param.first == shooterPositionParamHandle) {
                std::pair<double, double> position = decodePositionRec(param.second);
                MissileManagerSetter::setShooterPosition(*this, position);
                hasShooterPosition = true;
            }
            else if (param.first == targetPositionParamHandle) {
                std::pair<double, double> position = decodePositionRec(param.second);
                MissileManagerSetter::setTargetPosition(*this, position);
                hasTargetPosition = true;
            }
            /*
            TODO: Add the rest of the parameters
                missileCountParamHandle     Based on ship size, fire X number of missiles
                missileTypeParamHandle      Different missile types
                maxDistanceParamHandle      Missile max travel distance
                missileSpeedParamHandle     Missile speed
                lockOnDistanceParamHandle   Distance to missile to auto lock on target
                fireTimeParamHandle         Simulation time, might not be nessessary / get from somewhere else
            */
        }

        if (!hasShooterID || !hasTargetID || !hasShooterPosition || !hasTargetPosition) {
            std::wcerr << L"[ERROR] Missing required missile data!" << std::endl;
            return;
        }

        MissileManagerSetter::setFlagActiveMissile(*this, true);
        // Flag that valid missile data was received
        std::wcout << L"[INFO] Valid missile data received." << std::endl;
    }
}

void MissileManagerAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isRegulating = true;
    std::wcout << L"Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MissileManagerAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    isConstrained = true;
    std::wcout << L"Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MissileManagerAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) { //Used for time management
    std::wcout << L"[DEBUG] Time Advance Grant received: "
               << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

//getters and setters for attributes
double MissileManagerAmbassador::getCurrentAltitude() const {
    return currentAltitude;
}
void MissileManagerAmbassador::setCurrentAltitude(double altitude) {
    currentAltitude = altitude;
}

double MissileManagerAmbassador::getCurrentSpeed() const {
    return currentSpeed;
}
void MissileManagerAmbassador::setCurrentSpeed(const double& speed) {
    currentSpeed = speed;
}

double MissileManagerAmbassador::getCurrentFuelLevel() const {
    return currentFuelLevel;
}
void MissileManagerAmbassador::setCurrentFuelLevel(const double& fuelLevel) {
    currentFuelLevel = fuelLevel;
}

std::wstring MissileManagerAmbassador::getCurrentPosition() const {
    return currentPosition;
}
void MissileManagerAmbassador::setCurrentPosition(const std::wstring& position) {
    currentPosition = position;
}

double MissileManagerAmbassador::getCurrentDistance() const {
    return currentDistance;
}
void MissileManagerAmbassador::setCurrentDistance(const double& distance) {
    currentDistance = distance;
}
int MissileManagerAmbassador::getNumberOfRobots() const {
    return numberOfRobots;
}
void MissileManagerAmbassador::setNumberOfRobots(const int& robots) {
    numberOfRobots = robots;
}
// general get and set functions
std::wstring MissileManagerAmbassador::getSyncLabel() const {
    return syncLabel;
}
std::wstring MissileManagerAmbassador::getFederateName() const {
    return federateName;
}
void MissileManagerAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

//get and set for fire interaction
rti1516e::InteractionClassHandle MissileManagerAmbassador::getFireRobotHandle() const {return fireRobotHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getFireRobotHandleParam() const {return fireParamHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getTargetParam() const {return targetParamHandle;}
rti1516e::ParameterHandle MissileManagerAmbassador::getStartPosRobot() const {return startPosRobot;}
void MissileManagerAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {fireRobotHandle = handle;}
void MissileManagerAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {fireParamHandle = handle;}
void MissileManagerAmbassador::setTargetParam(const rti1516e::ParameterHandle& handle) {targetParamHandle = handle;}
void MissileManagerAmbassador::setStartPosRobot(const rti1516e::ParameterHandle& handle) {startPosRobot = handle;}