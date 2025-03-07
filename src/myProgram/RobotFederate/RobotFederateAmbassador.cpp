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

        rti1516e::HLAinteger32BE paramValueFire;
        paramValueFire.decode(itFireParamHandle->second);
        std::wcout << L"Instance " << instance << L": Fire robot: " << paramValueFire.get() << std::endl;
        rti1516e::HLAunicodeString paramValueTargetShip;
        paramValueTargetShip.decode(itTargetParamHandle->second);
        std::wcout << L"Instance " << instance << L": Target ship: " << paramValueTargetShip.get() << std::endl;
        TargetFederate = paramValueTargetShip.get();
        startFire = true;
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