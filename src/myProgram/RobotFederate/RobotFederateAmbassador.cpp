#include "RobotFederateAmbassador.h"


MyFederateAmbassador::MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance)
    : _rtiAmbassador(rtiAmbassador), instance(instance) {
    _expectedShipName = L"ShipFederate " + std::to_wstring(instance);
    startTime = std::chrono::high_resolution_clock::now();
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
    
    // Store the ship's class
    _shipInstances[theObject] = theObjectClass;

    // Request the initial values of all attributes
    try {
        rti1516e::AttributeHandleSet attributes;
        _rtiAmbassador->requestAttributeValueUpdate(theObject, attributes, rti1516e::VariableLengthData());
        std::wcout << L"Requested initial attributes for ObjectInstance: " << theObject << std::endl;
    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"Error requesting initial attributes: " << e.what() << std::endl;
    }
}

void MyFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
    
    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
    std::wstring tempShipID;

    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);

        if (_shipInstances.find(theObject) != _shipInstances.end()) {
            try {
                // Attributes retrieval
                auto itShipTag = theAttributes.find(attributeHandleShipTag);
                auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
                auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
                auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
                auto itShipSize = theAttributes.find(attributeHandleShipSize);
                auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfRobots);
                auto itShipLocked = theAttributes.find(attributeHandleShipLocked);

                if (itShipLocked != theAttributes.end()) {
                    std::wcout << L"[DEBUG] Found ShipLocked attribute" << std::endl;

                    rti1516e::HLAunicodeString attributeValueShipLocked;
                    attributeValueShipLocked.decode(itShipLocked->second);
                    std::wstring lockedByRobot = attributeValueShipLocked.get();

                    std::wcout << L"Instance " << theObject << L": Received Ship Locked: " << lockedByRobot << std::endl;

                    // If the ship is available (lockedByRobot is "EMPTY"), lock it
                    if (lockedByRobot == L"EMPTY") {
                        std::wcout << "[DEBUG] Locking ship for federate: " << federateName << std::endl;

                        rti1516e::AttributeHandleValueMap attributes;
                        attributes[attributeHandleShipLocked] = rti1516e::HLAunicodeString(federateName).encode();

                        try {
                            _rtiAmbassador->updateAttributeValues(theObject, attributes, rti1516e::VariableLengthData());
                            std::wcout << L"Ship is now locked by this robot: " << federateName << std::endl;
                        } catch (const rti1516e::Exception &e) {
                            std::wcerr << L"Error updating ShipLocked attribute: " << e.what() << std::endl;
                        }
                    } else {
                        // If the ship is already locked, print who has locked it
                        std::wcout << L"Ship is already locked by: " << lockedByRobot << std::endl;
                    }

                    // If the ship is locked by another federate, ignore further updates
                    if (lockedByRobot != federateName) {
                        std::wcout << "[DEBUG] Ship locked by another federate. Ignoring." << std::endl;
                        return;
                    }
                }

                // Check if ShipTag attribute exists and store its value
                if (itShipTag != theAttributes.end()) {
                    std::wcout << "[DEBUG] Found ShipTag attribute" << std::endl;

                    rti1516e::HLAunicodeString attributeValueShipTag;
                    attributeValueShipTag.decode(itShipTag->second);
                    _targetShipID = attributeValueShipTag.get();

                    std::wcout << L"Set _targetShipID to: " << _targetShipID << std::endl;
                }
                if (itShipPosition != theAttributes.end()) {
                    rti1516e::HLAunicodeString attributeValueShipPosition;
                    attributeValueShipPosition.decode(itShipPosition->second);
                    std::wcout << L"Instance " << instance << L": Received Ship Position: " << attributeValueShipPosition.get() << std::endl;
                    shipPosition = attributeValueShipPosition.get();
                }
                if (itFutureShipPosition != theAttributes.end()) {
                    rti1516e::HLAunicodeString attributeValueFutureShipPosition;
                    attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
                    std::wcout << L"Instance " << instance << L": Received Future Ship Position: " << attributeValueFutureShipPosition.get() << std::endl;
                    expectedShipPosition = attributeValueFutureShipPosition.get();
                }
                if (itShipSpeed != theAttributes.end()) {
                    rti1516e::HLAfloat64BE attributeValueShipSpeed;
                    attributeValueShipSpeed.decode(itShipSpeed->second);
                    std::wcout << L"Instance " << instance << L": Received Ship Speed: " << attributeValueShipSpeed.get() << std::endl;
                }
                if (itShipSize != theAttributes.end()) {
                    rti1516e::HLAfloat64BE attributeValueShipSize;
                    attributeValueShipSize.decode(itShipSize->second);
                    shipSize = attributeValueShipSize.get();
                    std::wcout << L"Instance " << instance << L": Received Ship Size: " << attributeValueShipSize.get() << std::endl;
                }
                if (itNumberOfRobots != theAttributes.end()) {
                    rti1516e::HLAinteger32BE attributeValueNumberOfRobots;
                    attributeValueNumberOfRobots.decode(itNumberOfRobots->second);
                    numberOfRobots = attributeValueNumberOfRobots.get();
                    std::wcout << L"Instance " << instance << L": Received Number of Robots: " << numberOfRobots << std::endl;
                }

                

                // Calculate distance and initial bearing between publisher and ship positions
                double initialBearing = _robot.calculateInitialBearingWstring(currentPosition, shipPosition);
                currentPosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
                currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
                currentAltitude = _robot.reduceAltitude(currentAltitude, currentSpeed, currentDistance);
                expectedFuturePosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);

                std::wcout << std::endl
                           << L"Instance " << instance << L": Robot Current Position: " << currentPosition << std::endl;
                std::wcout << L"Instance " << instance << L": Ship Current Position: " << shipPosition << std::endl;
                std::wcout << L"Instance " << instance << L": Robot Future Position: " << expectedFuturePosition << std::endl;
                std::wcout << L"Instance " << instance << L": Ship Future Position: " << expectedShipPosition << std::endl;
                std::wcout << L"Instance " << instance << L": Robot Current Altitude: " << currentAltitude << std::endl;
                std::wcout << L"Instance " << instance << L": Distance between robot and ship: " << currentDistance << " meters" << std::endl;

                if (currentDistance < 1000) {
                    std::wcout << L"Instance " << instance << L": Robot is within 1000 meters of target" << std::endl;
                    if (currentDistance < 100) {
                        std::wcout << L"Instance " << instance << L": Robot is within 100 meters of target" << std::endl;
                        if (currentDistance < 50) {
                            auto endTime = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double> realTimeDuration = endTime - startTime;
                            double realTime = realTimeDuration.count();

                            std::vector<std::wstring> finalData;
                            finalData.push_back(L"--------------------------------------------");
                            finalData.push_back(L"Instance : " + std::to_wstring(instance));
                            finalData.push_back(L"Last Distance : " + std::to_wstring(currentDistance) + L" meters");
                            finalData.push_back(L"Last Altitude : " + std::to_wstring(currentAltitude) + L" meters");
                            finalData.push_back(L"Last Speed : " + std::to_wstring(currentSpeed) + L" m/s");
                            finalData.push_back(L"Last position for robot : " + currentPosition);
                            finalData.push_back(L"Last position for ship : " + shipPosition);
                            finalData.push_back(L"Target ship size : " + std::to_wstring(shipSize) + L" m^3");
                            finalData.push_back(L"Robots remaining : " + std::to_wstring(numberOfRobots));
                            finalData.push_back(L"Simulation time : " + std::to_wstring(simulationTime) + L" seconds");
                            finalData.push_back(L"Real time : " + std::to_wstring(realTime) + L" seconds");
                            finalData.push_back(L"--------------------------------------------");

                            // Write the final data to a text file
                            std::ofstream outFile;
                            outFile.open("/usr/OjOpenRTI/OpenRTI/src/myProgram/log/finalData.txt", std::ios::app);
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
                            currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
                            std::wcout << L"Instance " << instance << L": Distance between robot and ship before last contact: " << currentDistance << " meters" << std::endl;
                            hitStatus = true;
                        }
                    }
                }
            } catch (const std::invalid_argument &e) {
                std::wcerr << L"Instance " << instance << L": Invalid position format" << std::endl;
            } catch (const rti1516e::Exception& e) {
                std::wcerr << L"RTI Exception: " << e.what() << std::endl;
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
    rti1516e::SupplementalReceiveInfo receiveInfo) 
{
    std::wcout << L"[DEBUG] 1" << std::endl;
    if (interactionClassHandle == hitEventHandle) {
        std::wcout << L"Processing HitEvent." << std::endl;

        std::wstring receivedRobotID, receivedShipID;

        
        // Might be unnecessary.
        auto iterRobot = parameterValues.find(robotIDParam);
        if (iterRobot != parameterValues.end()) {
            rti1516e::HLAunicodeString robotIDDecoder;
            robotIDDecoder.decode(iterRobot->second);
            receivedRobotID = robotIDDecoder.get();
            if(receivedRobotID != federateName) {
                return;
            }
        }

        auto iterShip = parameterValues.find(shipIDParam);
        if (iterShip != parameterValues.end()) {
            rti1516e::HLAunicodeString shipIDDecoder;
            shipIDDecoder.decode(iterShip->second);
            receivedShipID = shipIDDecoder.get();
            _targetShipID = receivedShipID;
        }
        
        // This is template. Make something cool with it later.
        auto iterDamage = parameterValues.find(damageParam);
        if (iterDamage != parameterValues.end()) {
            rti1516e::HLAinteger32BE damageDecoder;
            damageDecoder.decode(iterDamage->second);
            damageAmount = damageDecoder.get();
        }
    }
}

std::wstring MyFederateAmbassador::getSyncLabel() const {
    return syncLabel;
}

std::wstring MyFederateAmbassador::getFederateName() const {
    return federateName;
}

void MyFederateAmbassador::setFederateName(std::wstring name) {
    federateName = name;
}

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