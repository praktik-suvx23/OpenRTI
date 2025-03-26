#include "MissileFederateAmbassador.h"

MissileFederateAmbassador::MissileFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {
}

MissileFederateAmbassador::~MissileFederateAmbassador() {}

void MissileFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"[DEBUG] Synchronization point announced: " << label << std::endl;
}

void MissileFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    objectInstanceClassMap[theObject] = theObjectClass;
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
    std::wcout << L"[DEBUG] Reflecting attributes for object: " << theObject << std::endl;

    rti1516e::ObjectClassHandle objectClass;
    if (objectInstanceClassMap.find(theObject) != objectInstanceClassMap.end()) {
        objectClass = objectInstanceClassMap.at(theObject);
    } else {
        std::wcerr << L"[ERROR] Object class not found for object: " << theObject << std::endl;
        return;
    }

    // Loop through attributes
    for (const auto& attr : theAttributes) {
        rti1516e::AttributeHandle attributeHandle = attr.first;
        const rti1516e::VariableLengthData& encodedData = attr.second;

        try {
            if (objectClass == objectClassHandleShip) {
                if (attributeHandle == attributeHandleShipFederateName) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Ship Federate Name: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleShipTeam) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Ship Team: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleShipPosition) {
                    std::pair<double, double> position = decodePositionRec(encodedData);
                    std::wcout << L"[INFO] Ship Position: (" << position.first << L", " << position.second << L")" << std::endl;
                } else if (attributeHandle == attributeHandleShipSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Ship Speed: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleShipSize) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Ship Size: " << value.get() << std::endl;
                }
            } 
            else if (objectClass == objectClassHandleMissile) {
                if (attributeHandle == attributeHandleMissileID) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Missile ID: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissileTeam) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Missile Team: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissilePosition) {
                    std::pair<double, double> position = decodePositionRec(encodedData);
                    std::wcout << L"[INFO] Missile Position: (" << position.first << L", " << position.second << L")" << std::endl;
                } else if (attributeHandle == attributeHandleMissileAltitude) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Missile Altitude: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissileSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    std::wcout << L"[INFO] Missile Speed: " << value.get() << std::endl;
                }
            } else {
                std::wcerr << L"[WARNING] Unrecognized object class: " << objectClass << std::endl;
            }
        } catch (const rti1516e::Exception& e) {
            std::wcerr << L"[ERROR] Failed to decode attribute: " << e.what() << std::endl;
        }
    }
}

void MissileFederateAmbassador::receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) {
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
    std::wcout << L"[DEBUG] Recieved interaction." << std::endl;
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
            missilePosition = tempMissileStartPosition;
            missileTargetPosition = tempMissileTargetPosition;
            numberOfMissilesFired = tempNumberOfMissilesFired;
            
            for (const auto& missile : missiles) {
                if (missile.structInitialTargetPosition == tempMissileTargetPosition) {
                    std::wcout << L"[INFO] Target already assigned to a missile. Skipping creation." << std::endl;
                    return; // Exit the function to prevent creating a new missile
                }
            }

            createNewMissileObject(numberOfMissilesFired);

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
                std::pair<double, double> missilePosition = decodePositionRec(param.second);
                std::wcout << L"[INFO] Missile Position: " << missilePosition.first << ", "
                << missilePosition.second << std::endl;
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
}

void MissileFederateAmbassador::addNewMissile(rti1516e::ObjectInstanceHandle objectInstanceHandle)
{
    std::wcout << L"[INFO] addNewMissile - " << objectInstanceHandle << std::endl;
    missiles.emplace_back(objectInstanceHandle);
    missileMap[objectInstanceHandle] = missiles.size() - 1;
}

void MissileFederateAmbassador::removeMissileObject(rti1516e::ObjectInstanceHandle missileInstanceHandle)
{
    std::wcout << L"[INFO] removeMissileObject - " << missileInstanceHandle << std::endl;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t>::iterator it = missileMap.find(missileInstanceHandle);
    if (it != missileMap.end()) {
        size_t index = it->second;
        if (index < missiles.size() - 1) {
            missiles[index] = std::move(missiles.back());
            missileMap[missiles[index].objectInstanceHandle] = index;
        }
        missiles.pop_back();
        missileMap.erase(it);
    }
}

void MissileFederateAmbassador::createNewMissileObject(int numberOfNewMissiles)
{
    try {
        for(int i = 0; i < numberOfNewMissiles; i++)
        {
            rti1516e::ObjectInstanceHandle objectInstanceHandle = _rtiAmbassador->registerObjectInstance(objectClassHandleMissile);
            addNewMissile(objectInstanceHandle);

            missiles.back().structMissileID = L"Missile" + std::to_wstring(missileCounter++);
            missiles.back().structMissileTeam = missileTeam;
            missiles.back().structMissilePosition = missilePosition;
            missiles.back().structInitialTargetPosition = missileTargetPosition;
            missiles.back().structMissileAltitude = 0.0;
            missiles.back().structMissileSpeed = 4000; // m/s
            missiles.back().structMissileStartTime = std::chrono::high_resolution_clock::now(); // Unsure if this needs to be an Attribute, try without for now.
            missiles.back().structInitialBearing = calculateInitialBearingDouble(
                missilePosition.first, 
                missilePosition.second, 
                missileTargetPosition.first, 
                missileTargetPosition.second);
            missiles.back().structMissileDistanceToTarget = calculateDistance(
                missilePosition, 
                missileTargetPosition,
                missiles.back().structMissileAltitude);

            rti1516e::HLAfixedRecord missilePositionRecord;
            missilePositionRecord.appendElement(rti1516e::HLAfloat64BE(missiles.back().structMissilePosition.first));
            missilePositionRecord.appendElement(rti1516e::HLAfloat64BE(missiles.back().structMissilePosition.second));

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleMissileID] = rti1516e::HLAunicodeString(missiles.back().structMissileID).encode();
            attributeValues[attributeHandleMissileTeam] = rti1516e::HLAunicodeString(missiles.back().structMissileTeam).encode();
            attributeValues[attributeHandleMissilePosition] = missilePositionRecord.encode();
            attributeValues[attributeHandleMissileAltitude] = rti1516e::HLAfloat64BE(missiles.back().structMissileAltitude).encode();
            attributeValues[attributeHandleMissileSpeed] = rti1516e::HLAfloat64BE(missiles.back().structMissileSpeed).encode();

            _rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] createNewMissileObject - Exception: " << e.what() << std::endl;
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
    //std::wcout << L"[DEBUG] Time Advance Grant received: "
    //           << dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime).getTime() << std::endl;

    isAdvancing = false;  // Allow simulation loop to continue
}

// Getter & setters for time management
std::chrono::time_point<std::chrono::high_resolution_clock> MissileFederateAmbassador::getStartTime() const {
    return startTime;
}
void MissileFederateAmbassador::setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time) {
    startTime = time;
}

bool MissileFederateAmbassador::getIsRegulating() const {
    return isRegulating;
}

bool MissileFederateAmbassador::getIsConstrained() const {
    return isConstrained;
}

bool MissileFederateAmbassador::getIsAdvancing() const {
    return isAdvancing;
}
void MissileFederateAmbassador::setIsAdvancing(bool advancing) {
    isAdvancing = advancing;
}

// Getters and setters for missile objects
std::vector<Missile>& MissileFederateAmbassador::getMissiles() {
    return missiles;
}
void MissileFederateAmbassador::setMissiles(const std::vector<Missile>& missile) {
    missiles = missile;
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

rti1516e::AttributeHandle MissileFederateAmbassador::getAttributeHandleMissileID() const {
    return attributeHandleMissileID;
}
void MissileFederateAmbassador::setAttributeHandleMissileID(const rti1516e::AttributeHandle& handle) {
    attributeHandleMissileID = handle;
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

// Getter and setter for targetHit Interaction
rti1516e::InteractionClassHandle MissileFederateAmbassador::getInteractionClassTargetHit() const {
    return interactionClassTargetHit;
}
void MissileFederateAmbassador::setInteractionClassTargetHit(const rti1516e::InteractionClassHandle& handle) {
    interactionClassTargetHit = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamTargetHitID() const {
    return parameterHandleTargetHitID;
}
void MissileFederateAmbassador::setParamTargetHitID(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitID = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamTargetHitTeam() const {
    return parameterHandleTargetHitTeam;
}
void MissileFederateAmbassador::setParamTargetHitTeam(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitTeam = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamTargetHitPosition() const {
    return parameterHandleTargetHitPosition;
}
void MissileFederateAmbassador::setParamTargetHitPosition(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitPosition = handle;
}

rti1516e::ParameterHandle MissileFederateAmbassador::getParamTargetHitDestroyed() const {
    return parameterHandleTargetHitDestroyed;
}
void MissileFederateAmbassador::setParamTargetHitDestroyed(const rti1516e::ParameterHandle& handle) {
    parameterHandleTargetHitDestroyed = handle;
}


// Variables used in receiveInteraction
std::wstring MissileFederateAmbassador::getShooterID() const {
    return shooterID;
}

std::wstring MissileFederateAmbassador::getMissileTeam() const {
    return missileTeam;
}

std::pair<double, double> MissileFederateAmbassador::getMissilePosition() const {
    return missilePosition;
}
void MissileFederateAmbassador::setMissilePosition(const std::pair<double, double>& position) {
    missilePosition = position;
}

std::pair<double, double> MissileFederateAmbassador::getMissileTargetPosition() const {
    return missileTargetPosition;
}

int MissileFederateAmbassador::getNumberOfMissilesFired() const {
    return numberOfMissilesFired;
}

double MissileFederateAmbassador::getSimulationTime() const {
    return simulationTime;
}

// general get and set functions
std::wstring MissileFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}