#include "MissileFederateAmbassador.h"
#include "../include/decodePosition.h"


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
            auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfMissiles);
    
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
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    std::wcout << L"[DEBUG] Received interaction: " << interactionClassHandle << std::endl;

    if (interactionClassSetupSimulation == interactionClassHandle) {
        auto itTimeScaleFactor = parameterValues.find(parameterHandleSimulationTime);
        if (itTimeScaleFactor == parameterValues.end()) {
            std::wcerr << L"Missing parameter in setup simulation interaction" << std::endl;
            return;
        }

        rti1516e::HLAfloat64BE timeScaleFactor;
        timeScaleFactor.decode(itTimeScaleFactor->second);
        simulationTime = timeScaleFactor.get();

        std::wcout << L"Time scale factor: " << simulationTime << std::endl;
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
    std::wcout << L"[DEBUG] Recieved interaction: " << interactionClassHandle << std::endl;
    if (interactionClassHandle == interactionClassFireMissile) {        
        // Variables to temporary store the received parameters
        std::wstring tempShooterID, tempMissileTeam;
        std::pair<double, double> tempMissileStartPosition, tempMissileTargetPosition;
        int tempNumberOfMissilesFired;
        bool hasShooterID = false, hasMissileTeam = false, hasMissileStartPosition = false, hasMissileTargetPosition = false, hasNumberOfMissilesFired = false;
    
        for (const auto& param : parameterValues) {
            try {
                if (param.first == parameterHandleShooterID) {
                    rti1516e::HLAunicodeString tempValue;
                    tempValue.decode(param.second);
                    tempShooterID = tempValue.get();
                    std::wcout << L"[INFO] Shooter ID: " << tempShooterID << std::endl;
                    hasShooterID = true;
                } else if (param.first == parameterHandleMissileTeam) {
                    rti1516e::HLAunicodeString tempValue;
                    tempValue.decode(param.second);
                    tempMissileTeam = tempValue.get();
                    std::wcout << L"[INFO] Missile Team: " << tempMissileTeam << std::endl;
                    hasMissileTeam = true;
                } else if (param.first == parameterHandleMissileStartPosition) {
                    tempMissileStartPosition = decodePositionRec(param.second);
                    std::wcout << L"[INFO] Missile Start Position: (" 
                                << tempMissileStartPosition.first << L", " 
                                << tempMissileStartPosition.second << L")" << std::endl;
                    hasMissileStartPosition = true;
                } else if (param.first == parameterHandleMissileTargetPosition) {
                    tempMissileTargetPosition = decodePositionRec(param.second);
                    std::wcout << L"[INFO] Missile Target Position: (" 
                                << tempMissileTargetPosition.first << L", " 
                                << tempMissileTargetPosition.second << L")" << std::endl;
                    hasMissileTargetPosition = true;
                } else if (param.first == parameterHandleNumberOfMissilesFired) {
                    rti1516e::HLAinteger32BE tempValue;
                    tempValue.decode(param.second);
                    tempNumberOfMissilesFired = tempValue.get();
                    std::wcout << L"[INFO] Number of Missiles Fired: " << tempNumberOfMissilesFired << std::endl;
                    hasNumberOfMissilesFired = true;
                } else {
                    std::wcerr << L"[WARNING] Unknown parameter handle: " << param.first << std::endl;
                }
            } catch (const rti1516e::Exception& e) {
                std::wcerr << L"[ERROR] Failed to decode parameter: " << e.what() << std::endl;
            }
        }
    
        if (hasShooterID && hasMissileTeam && hasMissileStartPosition && hasMissileTargetPosition && hasNumberOfMissilesFired) {
            std::wcout << L"[INFO] All parameters received. Updating variables." << std::endl;
    
            shooterID = tempShooterID;
            missileTeam = tempMissileTeam;
            missileStartPosition = tempMissileStartPosition;
            missileTargetPosition = tempMissileTargetPosition;
            numberOfMissilesFired = tempNumberOfMissilesFired;
            createNewMissile = true;
        } else {
            std::wcerr << L"[ERROR] Missing parameters in fire interaction. Variables not updated." << std::endl;
            return;
        }
    }
    else if (interactionClassHandle == interactionClassMissileFlight) {
        for(const auto& param : parameterValues) {
            if (param.first == parameterHandleMissileFlightID) {
                rti1516e::HLAunicodeString tempValue;
                tempValue.decode(param.second);
                std::wstring missileID = tempValue.get();
                std::wcout << L"[INFO] Missile ID: " << missileID << std::endl;
            } else if (param.first == parameterHandleMissileFlightTeam) {
                rti1516e::HLAunicodeString tempValue;
                tempValue.decode(param.second);
                std::wstring missileTeam = tempValue.get();
                std::wcout << L"[INFO] Missile Team: " << missileTeam << std::endl;
            } else if (param.first == parameterHandleMissileStartPosition) {
            //    std::pair<double, double> missilePosition = decodePosition(param.second);
            //    std::wcout << L"[INFO] Missile Position: " << missilePosition << std::endl;
            } else if (param.first == parameterHandleMissileFlightAltitude) {
                rti1516e::HLAfloat64BE tempValue;
                tempValue.decode(param.second);
                double missileAltitude = tempValue.get();
                std::wcout << L"[INFO] Missile Altitude: " << missileAltitude << std::endl;
            } else if (param.first == parameterHandleMissileSpeed) {
                rti1516e::HLAfloat64BE tempValue;
                tempValue.decode(param.second);
                double missileSpeed = tempValue.get();
                std::wcout << L"[INFO] Missile Speed: " << missileSpeed << std::endl;
            } else if (param.first == parameterHandleMissileFlightLockOnTargetID) {
                rti1516e::HLAunicodeString tempValue;
                tempValue.decode(param.second);
                std::wstring lockOnTargetID = tempValue.get();
                std::wcout << L"[INFO] Lock on Target ID: " << lockOnTargetID << std::endl;
            } else if (param.first == parameterHandleMissileFlightHitTarget) {
                rti1516e::HLAboolean tempValue;
                tempValue.decode(param.second);
                bool hitTarget = tempValue.get();
                std::wcout << L"[INFO] Hit Target: " << hitTarget << std::endl;
            } else if (param.first == parameterHandleMissileFlightDestroyed) {
                rti1516e::HLAboolean tempValue;
                tempValue.decode(param.second);
                bool destroyed = tempValue.get();
                std::wcout << L"[INFO] Missile Destroyed: " << destroyed << std::endl;
            } else {
                std::wcerr << L"[WARNING] Unknown parameter handle: " << param.first << std::endl;
            }
            /* TODO: Something with these interactions. Send some interaction when close enough to the ship. */
        }

    }
    /* Remove 'fireRobotHandle' when interactionClassFireMissile works*/
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
rti1516e::InteractionClassHandle MissileFederateAmbassador::getInteractionClassFireMissile() const {
    return interactionClassFireMissile;
}
void MissileFederateAmbassador::setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle) {
    interactionClassFireMissile = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamShooterID() const {
    return parameterHandleShooterID;
}
void MissileFederateAmbassador::setParamShooterID(const rti1516e::ParameterHandle& handle) {
    parameterHandleShooterID = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileTeam() const {
    return parameterHandleMissileTeam;
}
void MissileFederateAmbassador::setParamMissileTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTeam = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileStartPosition() const {
    return parameterHandleMissileStartPosition;
}
void MissileFederateAmbassador::setParamMissileStartPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileStartPosition = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileTargetPosition() const {
    return parameterHandleMissileTargetPosition;
}
void MissileFederateAmbassador::setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileTargetPosition = handle;
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

// Getter and setter functions for interaction class MissileFlight
rti1516e::InteractionClassHandle MissileFederateAmbassador::getInteractionClassMissileFlight() const {
    return interactionClassMissileFlight;
}
void MissileFederateAmbassador::setInteractionClassMissileFlight(const rti1516e::InteractionClassHandle& handle) {
    interactionClassMissileFlight = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightID() const {
    return parameterHandleMissileFlightID;
}
void MissileFederateAmbassador::setParamMissileFlightID(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightID = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightTeam() const {
    return parameterHandleMissileFlightTeam;
}
void MissileFederateAmbassador::setParamMissileFlightTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightTeam = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightPosition() const {
    return parameterHandleMissileFlightPosition;
}
void MissileFederateAmbassador::setParamMissileFlightPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightPosition = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightAltitude() const {
    return parameterHandleMissileFlightAltitude;
}
void MissileFederateAmbassador::setParamMissileFlightAltitude(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightAltitude = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightSpeed() const {
    return parameterHandleMissileFlightSpeed;
}
void MissileFederateAmbassador::setParamMissileFlightSpeed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightSpeed = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightLockOnTargetID() const {
    return parameterHandleMissileFlightLockOnTargetID;
}
void MissileFederateAmbassador::setParamMissileFlightLockOnTargetID(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightLockOnTargetID = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightHitTarget() const {
    return parameterHandleMissileFlightHitTarget;
}
void MissileFederateAmbassador::setParamMissileFlightHitTarget(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightHitTarget = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamMissileFlightDestroyed() const {
    return parameterHandleMissileFlightDestroyed;
}
void MissileFederateAmbassador::setParamMissileFlightDestroyed(const rti1516e::ParameterHandle& handle) {
    parameterHandleMissileFlightDestroyed = handle;
}

// Variables used in receiveInteraction
std::wstring MissileFederateAmbassador::getShooterID() const {
    return shooterID;
}

std::wstring MissileFederateAmbassador::getMissileTeam() const {
    return missileTeam;
}

std::pair<double, double> MissileFederateAmbassador::getMissileStartPosition() const {
    return missileStartPosition;
}

std::pair<double, double> MissileFederateAmbassador::getMissileTargetPosition() const {
    return missileTargetPosition;
}

int MissileFederateAmbassador::getNumberOfMissilesFired() const {
    return numberOfMissilesFired;
}

bool MissileFederateAmbassador::getCreateNewMissile() const {
    return createNewMissile;
}
void MissileFederateAmbassador::setCreateNewMissile(bool temp) {
    createNewMissile = temp;
}

double MissileFederateAmbassador::getSimulationTime() const {
    return simulationTime;
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

