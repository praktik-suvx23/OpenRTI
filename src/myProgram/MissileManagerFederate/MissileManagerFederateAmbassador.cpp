#include "MissileManagerFederateAmbassador.h"


MissileManagerAmbassador::MissileManagerAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}

MissileManagerAmbassador::~MissileManagerAmbassador() {}



void MissileManagerAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag) {
    syncLabel = label;
    std::wcout << L"Synchronization point announced: " << label << std::endl;
}

// Is this nessesary?
void MissileManagerAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName) {
    std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    _shipInstances[theObject] = theObjectClass;
}

// Recreate to work with objects instead of several federates
void MissileManagerAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const & theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    
/*
    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
    std::wstring tempShipID;

    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        if (attributeValueFederateName.get() != _expectedShipName) {
            return;
        } else {
            std::wcout << L"Instance " << instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
        }
        if (_shipInstances.find(theObject) != _shipInstances.end()) {
            auto itShipTag = theAttributes.find(attributeHandleShipTag);
            auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
            auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
            auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
            auto itShipSize = theAttributes.find(attributeHandleShipSize);
            auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfRobots);
    
            if (itShipTag != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipTag;
                attributeValueShipTag.decode(itShipTag->second);
            }
            if (itShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipPosition;
                attributeValueShipPosition.decode(itShipPosition->second);
                shipPosition = attributeValueShipPosition.get();
            }
            if (itFutureShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueFutureShipPosition;
                attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
                expectedShipPosition = attributeValueFutureShipPosition.get();
            }
            if (itShipSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSpeed;
                attributeValueShipSpeed.decode(itShipSpeed->second);
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
    
            // Calculate distance and initial bearing between publisher and ship positions
            if (startFire) {
                try {
                    double initialBearing = _robot.calculateInitialBearingWstring(getCurrentPosition(), shipPosition);
                    setCurrentPosition(_robot.calculateNewPosition(getCurrentPosition(), getCurrentSpeed(), initialBearing));
                    setCurrentDistance(_robot.calculateDistance(getCurrentPosition(), shipPosition, getCurrentAltitude()));
                    setCurrentAltitude(_robot.reduceAltitude(getCurrentAltitude(), getCurrentSpeed(), getCurrentDistance()));
                    expectedFuturePosition = _robot.calculateNewPosition(getCurrentPosition(), getCurrentSpeed(), initialBearing);
                    std::wcout << std::endl
                               << L"Instance " << instance << L": Robot Current Position: " << getCurrentPosition() << std::endl;
                    std::wcout << L"Instance " << instance << L": Ship Current Position: " << shipPosition << std::endl;
                    std::wcout << L"Instance " << instance << L": Robot Future Position: " << expectedFuturePosition << std::endl;
                    std::wcout << L"Instance " << instance << L": Ship Future Position: " << expectedShipPosition << std::endl;
                    std::wcout << L"Instance " << instance << L": Robot Current Altitude: " << getCurrentAltitude() << std::endl;
                    std::wcout << L"Instance " << instance << L": Distance between robot and ship: " << getCurrentDistance() << " meters" << std::endl;
                    if (getCurrentDistance() < 1000) {
                        std::wcout << L"Instance " << instance << L": Robot is within 1000 meters of target" << std::endl;
                        if (getCurrentDistance() < 100) {
                            std::wcout << L"Instance " << instance << L": Robot is within 100 meters of target" << std::endl;
                            if (getCurrentDistance() < 50) {
                                auto endTime = std::chrono::high_resolution_clock::now();
                                std::chrono::duration<double> realTimeDuration = endTime - startTime;
                                double realTime = realTimeDuration.count();
                                const auto& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime);
                                double simulationTime = floatTime.getTime();
                                hitStatus = true;
        
                                std::vector<std::wstring> finalData;
                                finalData.push_back(L"--------------------------------------------");
                                finalData.push_back(L"Instance : " + std::to_wstring(instance));
                                finalData.push_back(L"Last Distance : " + std::to_wstring(getCurrentDistance()) + L" meters");
                                finalData.push_back(L"Last Altitude : " + std::to_wstring(getCurrentAltitude()) + L" meters");
                                finalData.push_back(L"Last Speed : " + std::to_wstring(getCurrentSpeed()) + L" m/s");
                                finalData.push_back(L"Last position for robot : " + getCurrentPosition());
                                finalData.push_back(L"Last position for ship : " + shipPosition);
                                finalData.push_back(L"Target ship size : " + std::to_wstring(shipSize) + L" m^3");
                                finalData.push_back(L"Robots remaining : " + std::to_wstring(numberOfRobots));
                                finalData.push_back(L"Simulation time : " + std::to_wstring(simulationTime) + L" seconds");
                                finalData.push_back(L"Real time : " + std::to_wstring(realTime) + L" seconds");
                                finalData.push_back(L"--------------------------------------------");
        
                                // Write the final data to a text file
                                std::ofstream outFile;
                                outFile.open(DATA_LOG_PATH, std::ios::app);
                                if (outFile.is_open()) {
                                    for (const auto& entry : finalData) {
                                        outFile << std::string(entry.begin(), entry.end()) << std::endl;
                                    }
                                    outFile.close();
                                    std::wcout << L"Data successfully written to finalData.txt" << std::endl;
                                } else {
                                    std::wcerr << L"Unable to open file: finalData.txt" << std::endl;
                                }
        
                                std::wcout << L"Target reached" << std::endl;
                                setCurrentDistance(_robot.calculateDistance(getCurrentPosition(), shipPosition, getCurrentAltitude()));
                                std::wcout << L"Instance " << instance << L": Distance between robot and ship before last contact: " << getCurrentDistance() << " meters" << std::endl;
                                _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
                            }
                        }
                    }
                } catch (const std::invalid_argument &e) {
                    std::wcerr << L"Instance " << instance << L": Invalid position format" << std::endl;
                }
            }
        }
    }
*/
}

// Recreate to receive missile launch event, create new missile object and send it to the target
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

        std::wstring shooterID, targetID, missileType;
        double missileSpeed = 0.0, maxDistance = 0.0, lockOnDistance = 0.0, fireTime = 0.0;
        int missileCount = 0;
        PositionRec shooterPosition, targetPosition;

        for (const auto& param : parameterValues) {
            rti1516e::ParameterHandle paramHandle = param.first;
            const rti1516e::VariableLengthData& paramValue = param.second;

            if (paramHandle == shooterIDParamHandle) {
                shooterID = decodeString(paramValue);
            } 
            else if (paramHandle == targetIDParamHandle) {
                targetID = decodeString(paramValue);
            } 
            else if (paramHandle == shooterPositionParamHandle) {
                shooterPosition = decodePosition(paramValue);
            } 
            else if (paramHandle == targetPositionParamHandle) {
                targetPosition = decodePosition(paramValue);
            } 
            else if (paramHandle == missileCountParamHandle) {
                missileCount = decodeInteger(paramValue);
            } 
            else if (paramHandle == missileTypeParamHandle) {
                missileType = decodeString(paramValue);
            } 
            else if (paramHandle == maxDistanceParamHandle) {
                maxDistance = decodeDouble(paramValue);
            } 
            else if (paramHandle == missileSpeedParamHandle) {
                missileSpeed = decodeDouble(paramValue);
            } 
            else if (paramHandle == lockOnDistanceParamHandle) {
                lockOnDistance = decodeDouble(paramValue);
            } 
            else if (paramHandle == fireTimeParamHandle) {
                fireTime = decodeDouble(paramValue);
            }
        }

        // Flag that valid missile data was received
        std::wcout << L"[INFO] Valid missile data received, calling createMissiles()" << std::endl;

        // Call separate method to handle missile creation
        createMissiles(missileData);
    }
    /*
    std::wcout << L"[DEBUG] 1" << std::endl;
    if (interactionClassHandle == fireRobotHandle) {
        std::wcout << L"[DEBUG] 2" << std::endl;
        startFire = true;
    }
    */
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

// Getters and setters for the attributes handles
rti1516e::ObjectClassHandle MissileManagerAmbassador::getMyObjectClassHandle() const {
    return shipClassHandle;
}

void MissileManagerAmbassador::setMyObjectClassHandle(const rti1516e::ObjectClassHandle& handle) {
    shipClassHandle = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipTag() const {
    return attributeHandleShipTag;
}
void MissileManagerAmbassador::setAttributeHandleShipTag(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipTag = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipPosition() const {
    return attributeHandleShipPosition;
}
void MissileManagerAmbassador::setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipPosition = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleFutureShipPosition() const {
    return attributeHandleFutureShipPosition;
}
void MissileManagerAmbassador::setAttributeHandleFutureShipPosition(const rti1516e::AttributeHandle& handle) {
    attributeHandleFutureShipPosition = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSpeed() const {
    return attributeHandleShipSpeed;
}
void MissileManagerAmbassador::setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSpeed = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleShipSize() const {
    return attributeHandleShipSize;
}
void MissileManagerAmbassador::setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipSize = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleNumberOfRobots() const {
    return attributeHandleNumberOfRobots;
}
void MissileManagerAmbassador::setAttributeHandleNumberOfRobots(const rti1516e::AttributeHandle& handle) {
    attributeHandleNumberOfRobots = handle;
}

rti1516e::AttributeHandle MissileManagerAmbassador::getAttributeHandleFederateName() const {
    return attributeHandleShipFederateName;
}
void MissileManagerAmbassador::setAttributeHandleFederateName(const rti1516e::AttributeHandle& handle) {
    attributeHandleShipFederateName = handle;
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
rti1516e::InteractionClassHandle MissileManagerAmbassador::getFireRobotHandle() const {
    return fireRobotHandle;
}
void MissileManagerAmbassador::setFireRobotHandle(const rti1516e::InteractionClassHandle& handle) {
    fireRobotHandle = handle;
}

rti1516e::ParameterHandle MissileManagerAmbassador::getFireRobotHandleParam() const {
    return fireParamHandle;
}
void MissileManagerAmbassador::setFireRobotHandleParam(const rti1516e::ParameterHandle& handle) {
    fireParamHandle = handle;
}

// Interactions that are for the moment not implemented
bool MissileManagerAmbassador::getAssignedTarget() const {
    return assignedTarget;
}

bool MissileManagerAmbassador::getHitStatus() const {
    return hitStatus;
}

std::wstring MissileManagerAmbassador::getTargetShipID() const {
    return _targetShipID;
}
// is this in use???
std::wstring MissileManagerAmbassador::getShipID() const {
    return shipID;
}

int MissileManagerAmbassador::getDamageAmount() const {
    return damageAmount;
}

// Getter Methods
rti1516e::InteractionClassHandle MissileManagerAmbassador::getFireMissileHandle() const {
    return fireMissileHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getShooterIDParamHandle() const {
    return shooterIDParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getTargetIDParamHandle() const {
    return targetIDParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getShooterPositionParamHandle() const {
    return shooterPositionParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getTargetPositionParamHandle() const {
    return targetPositionParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getMissileCountParamHandle() const {
    return missileCountParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getMissileTypeParamHandle() const {
    return missileTypeParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getMaxDistanceParamHandle() const {
    return maxDistanceParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getMissileSpeedParamHandle() const {
    return missileSpeedParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getLockOnDistanceParamHandle() const {
    return lockOnDistanceParamHandle;
}
rti1516e::ParameterHandle MissileManagerAmbassador::getFireTimeParamHandle() const {
    return fireTimeParamHandle;
}
// Setter Methods
void MissileManagerAmbassador::setFireMissileHandle(rti1516e::InteractionClassHandle handle) {
    fireMissileHandle = handle;
}
void MissileManagerAmbassador::setShooterIDParamHandle(rti1516e::ParameterHandle handle) {
    shooterIDParamHandle = handle;
}
void MissileManagerAmbassador::setTargetIDParamHandle(rti1516e::ParameterHandle handle) {
    targetIDParamHandle = handle;
}
void MissileManagerAmbassador::setShooterPositionParamHandle(rti1516e::ParameterHandle handle) {
    shooterPositionParamHandle = handle;
}
void MissileManagerAmbassador::setTargetPositionParamHandle(rti1516e::ParameterHandle handle) {
    targetPositionParamHandle = handle;
}
void MissileManagerAmbassador::setMissileCountParamHandle(rti1516e::ParameterHandle handle) {
    missileCountParamHandle = handle;
}
void MissileManagerAmbassador::setMissileTypeParamHandle(rti1516e::ParameterHandle handle) {
    missileTypeParamHandle = handle;
}
void MissileManagerAmbassador::setMaxDistanceParamHandle(rti1516e::ParameterHandle handle) {
    maxDistanceParamHandle = handle;
}
void MissileManagerAmbassador::setMissileSpeedParamHandle(rti1516e::ParameterHandle handle) {
    missileSpeedParamHandle = handle;
}
void MissileManagerAmbassador::setLockOnDistanceParamHandle(rti1516e::ParameterHandle handle) {
    lockOnDistanceParamHandle = handle;
}
void MissileManagerAmbassador::setFireTimeParamHandle(rti1516e::ParameterHandle handle) {
    fireTimeParamHandle = handle;
}