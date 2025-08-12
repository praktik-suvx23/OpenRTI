#include "MissileFederateAmbassador.h"
#include <thread>

MissileFederateAmbassador::MissileFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) : _rtiAmbassador(rtiAmbassador) {
}

MissileFederateAmbassador::~MissileFederateAmbassador() {}

void MissileFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    if (label == L"MissilesCreated") {
        std::wcout << L"[INFO - SyncPoint] Federate synchronized at MissilesCreated." << std::endl;
        syncLabel = label;
    }
}

void MissileFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    //Used to map object instances to their class handles
    std::wcout << L"[DEBUG] Discovered object instance: " << theObject << L" of class: " << theObjectClass << std::endl;
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

    rti1516e::ObjectClassHandle objectClass;
    if (objectInstanceClassMap.find(theObject) != objectInstanceClassMap.end()) {
        objectClass = objectInstanceClassMap.at(theObject);
    } else {
        std::wcerr << L"[ERROR] Object class not found for object: " << theObject << std::endl;
        return;
    }
    std::wstring currentShipFederateName;
    std::wstring currentShipTeam;

    // Loop through attributes and get their values
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
                            newShip.structShipSize = 0; // Placeholder for size, for when that gets implemented
                            newShip.numberOfMissilesTargeting = 0;
                            ships.emplace_back(newShip);
                            shipsMap[theObject] = ships.size() - 1;
                            std::wcout << L"[DEBUG] Ship added to map: " << theObject << L" for missile " << missile.id << std::endl;
                        } 
                        else {
                            std::wcerr << L"[ERROR] Ship federate name or team is empty. Cannot add ship to map." << std::endl;
                            return;
                        } 
                    }
                
                    // Check if the ship is in the map
                    if (shipsMap.find(theObject) != shipsMap.end()) {

                        if (missile.lookingForTarget && !missile.targetFound && missile.groundDistanceToTarget < missile.distanceToTarget) {
                            std::wcout << L"[INFO]" << missile.team << L" missile " << missile.id << L" looking for target" << std::endl;
                            
                            double distanceBetween = calculateDistance(position, missile.position, missile.groundDistanceToTarget);
                            std::wcout << L"Distance between: " << distanceBetween << std::endl <<
                            L"Missile team: " << missile.team << std::endl <<
                            L"Current ship team: " << currentShipTeam << std::endl;

                            if (missile.team != currentShipTeam 
                                && currentShipTeam != L"" 
                                && shipsMap.find(theObject) != shipsMap.end()
                                && missile.targetID == ships[shipsMap[theObject]].structShipID
                                && !missile.targetFound 
                                ) {
                                    std::wcout << L"[INFO] Ship found for missile " << missile.id << L" at position " << position.first << L", " << position.second << std::endl;
                                    std::wcout << L"[INFO] Ship found in map" << std::endl;
                                    std::wcout << L"[INFO] Target found for missile " << missile.id << L" on ship " << currentShipFederateName << std::endl;
                                    missile.targetFound = true;
                                    missile.initialTargetPosition = position;
                                    missile.lookingForTarget = false;
                                    missile.targetID = ships[shipsMap[theObject]].structShipID;
                                    std::wstring debugEntry = missile.id + L" targeting " + ships[shipsMap[theObject]].structShipID;
                                    MissileTargetDebugOutPut.push_back(debugEntry);
                            } 
                            else if (missile.team == currentShipTeam) {
                                std::wcout << L"[INFO] Looking at own ship ignore targeting" << std::endl;
                                return; //Maybe
                            }
                            else {
                                std::wcout << L"[ERROR - reflectAttributeValues] Ship not found in map for find target" << std::endl;
                                std::wcout << std::endl << theObject << std::endl << std::endl;

                                std::wcout << std::endl << L"Ship ID: " << ships[shipsMap[theObject]].structShipID << std::endl << std::endl;
                            }
                        }
                        
                        if (missile.targetFound && missile.targetID == ships[shipsMap[theObject]].structShipID) {
                            auto it = shipsMap.find(theObject);
                            if (it == shipsMap.end()) {
                                std::wcerr << L"[ERROR] Ship not found in map, ship destroyed before impact" << std::endl;
                                missile.targetFound = false;
                                missile.lookingForTarget = true;
                            }
                            else {
                                std::wcout << L"[INFO] Missile adjusting course" << std::endl;
                                missile.initialTargetPosition = position;
                                missile.bearing = calculateInitialBearingDouble( 
                                    missile.position.first,
                                    missile.position.second,
                                    missile.initialTargetPosition.first,
                                    missile.initialTargetPosition.second);
                            }
                        }
                    }
                    
                } else if (attributeHandle == attributeHandleShipSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                } else if (attributeHandle == attributeHandleShipSize) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                }
            } 
            else if (objectClass == objectClassHandleMissile) {
                if (attributeHandle == attributeHandleMissileID) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                } else if (attributeHandle == attributeHandleMissileTeam) {
                    rti1516e::HLAunicodeString value;
                    value.decode(encodedData);
                    std::pair<double, double> position = decodePositionRec(encodedData); // Decode position from encoded data and get the coordinates
                } else if (attributeHandle == attributeHandleMissileAltitude) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
                } else if (attributeHandle == attributeHandleMissileSpeed) {
                    rti1516e::HLAfloat64BE value;
                    value.decode(encodedData);
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
            std::wcerr << L"[ERROR] Missing parameter in setup simulation interaction" << std::endl;
            return;
        }

        rti1516e::HLAfloat64BE timeScaleFactor;
        timeScaleFactor.decode(itTimeScaleFactor->second);
        simulationTime = timeScaleFactor.get();
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
    // Use this when functionalty for missile countermeasures is implemented
}

void MissileFederateAmbassador::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) {
    currentLogicalTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(theFederateTime).getTime();
    isRegulating = true;
    std::wcout << L"[INFO] Time Regulation Enabled: " << theFederateTime << std::endl;
}

void MissileFederateAmbassador::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) {
    currentLogicalTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(theFederateTime).getTime();
    isConstrained = true;
    std::wcout << L"[INFO] Time Constrained Enabled: " << theFederateTime << std::endl;
}

void MissileFederateAmbassador::timeAdvanceGrant(const rti1516e::LogicalTime &theTime) { //Used for time management

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

//Get and set used to store datavalues for create Missile struct used in creating new missiles
void MissileFederateAmbassador::setMissile(Missile missile) {
    this->missile = missile;
}
Missile MissileFederateAmbassador::getMissile() const {
    return missile;
}

void MissileFederateAmbassador::setLogType(loggingType newType) {
    logType = newType;
}

// general get and set functions
std::wstring MissileFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}



double MissileFederateAmbassador::getCurrentLogicalTime() const {
    return currentLogicalTime;
}