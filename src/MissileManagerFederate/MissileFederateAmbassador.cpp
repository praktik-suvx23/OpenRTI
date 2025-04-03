#include "MissileFederateAmbassador.h"
#include <thread>

MissileFederateAmbassador::MissileFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {
}

MissileFederateAmbassador::~MissileFederateAmbassador() {}

void MissileFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"InitialSync") {
        std::wcout << L"Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"SimulationSetupComplete") {
        std::wcout << L"Federate synchronized at SimulationSetupComplete." << std::endl;
        syncLabel = label;
    }
    if (label == L"BlueShipFederate") {
        std::wcout << L"Federate synchronized at BlueTeamSync." << std::endl;
        blueSyncLabel = label;
    }
    if (label == L"RedShipFederate") {
        std::wcout << L"Federate synchronized at RedTeamSync." << std::endl;
        redSyncLabel = label;
    }
}

void MissileFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    //std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
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
    //std::wcout << L"[DEBUG] Reflecting attributes for object: " << theObject << std::endl;

    rti1516e::ObjectClassHandle objectClass;
    if (objectInstanceClassMap.find(theObject) != objectInstanceClassMap.end()) {
        objectClass = objectInstanceClassMap.at(theObject);
    } else {
        std::wcerr << L"[ERROR] Object class not found for object: " << theObject << std::endl;
        return;
    }
    std::wstring currentShipFederateName;
    std::wstring currentShipTeam;
    // Loop through attributes
    for (const auto& attr : theAttributes) {
        rti1516e::AttributeHandle attributeHandle = attr.first;
        const rti1516e::VariableLengthData& encodedData = attr.second;

        try {
            if (objectClass == objectClassHandleShip) {
        
                if (attributeHandle == attributeHandleShipFederateName) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    currentShipFederateName = value.get();
                } 
                else if (attributeHandle == attributeHandleShipTeam) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    currentShipTeam = value.get();
                } 
                else if (attributeHandle == attributeHandleShipPosition) {
                    
                    std::pair<double, double> position = decodePositionRec(encodedData);
                    if (shipsMap.find(theObject) == shipsMap.end()) {

                        if (!currentShipFederateName.empty() && !currentShipTeam.empty()) {
                            TargetShips newShip(theObject);
                            newShip.structShipID = currentShipFederateName;
                            newShip.structShipTeam = currentShipTeam;
                            newShip.structShipSize = 0; // Placeholder for size
                            newShip.numberOfMissilesTargeting = 0;
                
                            ships.emplace_back(newShip);
                            shipsMap[theObject] = ships.size() - 1;
                            std::wcout << L"[DEBUG] Ship added to map: " << theObject << std::endl;
                            std::this_thread::sleep_for(std::chrono::milliseconds(5));
                        } 
                        else {
                            std::wcerr << L"[ERROR] Ship federate name or team is empty. Cannot add ship to map." << std::endl;
                            return;
                        } 
                    }
                
                    for (auto& missile : missiles) {
                        if (shipsMap.find(theObject) != shipsMap.end()) {
                            if (missile.LookingForTarget && !missile.TargetFound && missile.structMissileDistanceToTarget < 1200) {
                                std::wcout << L"[INFO]" << missile.structMissileTeam << L" missile " << missile.structMissileID << L" looking for target" << std::endl;
                                
                                double distanceBetween = calculateDistance(position, missile.structMissilePosition, missile.structMissileAltitude);
                                if (distanceBetween < 1200 
                                    && missile.structMissileTeam != currentShipTeam 
                                    && currentShipTeam != L""
                                    && shipsMap.find(theObject) != shipsMap.end()
                                    && ships[shipsMap[theObject]].numberOfMissilesTargeting < 2) {
                                    std::wcout << L"[INFO] Ship found for missile " << missile.structMissileID << L" at position " << position.first << L", " << position.second << std::endl;
                                    

                                        std::wcout << L"[INFO] Ship found in map" << std::endl;
                                        std::wcout << L"[INFO] Target found for missile " << missile.structMissileID << L" on ship " << currentShipFederateName << std::endl;
                                        ships[shipsMap[theObject]].numberOfMissilesTargeting++;
                                        missile.TargetFound = true;
                                        missile.structInitialTargetPosition = position;
                                        missile.LookingForTarget = false;
                                        missile.targetShipID = ships[shipsMap[theObject]].structShipID;
                                        
                                        std::wstring debugEntry = missile.structMissileID + L" targeting " + ships[shipsMap[theObject]].structShipID;
                                        MissileTargetDebugOutPut.push_back(debugEntry);
                                } 
                                else {
                                    std::wcout << L"[INFO] Ship not found in map" << std::endl;
                                }
                            }
                            
                            if (missile.TargetFound && missile.targetShipID == ships[shipsMap[theObject]].structShipID) {
                                auto it = shipsMap.find(theObject);
                                if (it == shipsMap.end()) {
                                    std::wcerr << L"[ERROR] Ship not found in map" << std::endl;
                                    missile.TargetFound = false;
                                    missile.LookingForTarget = true;
                                }
                                else {
                                    missile.structInitialTargetPosition = position; // Update target position
                                    missile.structInitialBearing = calculateInitialBearingDouble( // Calculate new bearing
                                        missile.structMissilePosition.first,
                                        missile.structMissilePosition.second,
                                        missile.structInitialTargetPosition.first,
                                        missile.structInitialTargetPosition.second);
                                }
                            }
                        }
                    }
                } else if (attributeHandle == attributeHandleShipSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Ship Speed: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleShipSize) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Ship Size: " << value.get() << std::endl;
                }
            } 
            else if (objectClass == objectClassHandleMissile) {
                if (attributeHandle == attributeHandleMissileID) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Missile ID: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissileTeam) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Missile Team: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissilePosition) {
                    std::pair<double, double> position = decodePositionRec(encodedData);
                    //std::wcout << L"[INFO] Missile Position: (" << position.first << L", " << position.second << L")" << std::endl;
                } else if (attributeHandle == attributeHandleMissileAltitude) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Missile Altitude: " << value.get() << std::endl;
                } else if (attributeHandle == attributeHandleMissileSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                    //std::wcout << L"[INFO] Missile Speed: " << value.get() << std::endl;
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

            createNewMissileObject(numberOfMissilesFired);

        } else {
            std::wcerr << L"[ERROR] Missing parameters in fire interaction. Variables not updated." << std::endl;
            return;
        }
    }
}

void MissileFederateAmbassador::addNewMissile(rti1516e::ObjectInstanceHandle objectInstanceHandle)
{
    std::wcout << L"[INFO] addNewMissile - " << objectInstanceHandle << std::endl;
    missiles.emplace_back(objectInstanceHandle);
    missileMap[objectInstanceHandle] = missiles.size() - 1;
}

bool MissileFederateAmbassador::removeMissileObject(rti1516e::ObjectInstanceHandle missileInstanceHandle)
{
    auto it = missileMap.find(missileInstanceHandle);
    if (it == missileMap.end()) {
        std::wcerr << L"[ERROR] Attempted to remove a non-existent missile: " << missileInstanceHandle << std::endl;
        return false;
    }

    size_t index = it->second;

    // Mark the missile as being removed
    missiles[index].TargetDestroyed = true;

    // Move the last missile to the current index and update the map
    if (index != missiles.size() - 1) {
        missiles[index] = std::move(missiles.back());
        missileMap[missiles[index].objectInstanceHandle] = index;
    }

    // Remove the last element and erase the map entry
    missiles.pop_back();
    missileMap.erase(missileInstanceHandle);

    _rtiAmbassador->deleteObjectInstance(missileInstanceHandle, rti1516e::VariableLengthData());

    std::wcout << L"[DEBUG] Missile removed: " << missileInstanceHandle << std::endl;
    return true;
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
            missiles.back().structMissileSpeed = 4000; // m/s is this correct?
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
            
            missiles.back().structMissileInitialDistanceToTarget = missiles.back().structMissileDistanceToTarget;

            missiles.back().structMissileHeightAchieved = false;

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


//Wanted Heigt for altitude 
double MissileFederateAmbassador::getWantedHeight() const {
    return wantedHeight;
}
void MissileFederateAmbassador::setWantedHeight(double height) {
    wantedHeight = height;
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
std::wstring MissileFederateAmbassador::getRedSyncLabel() const {
    return redSyncLabel;
}
std::wstring MissileFederateAmbassador::getBlueSyncLabel() const {
    return blueSyncLabel;
}

std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> MissileFederateAmbassador::getShips() const {
    return shipsMap;
}
std::vector<TargetShips>& MissileFederateAmbassador::getShipsVector() {
    return ships;
}