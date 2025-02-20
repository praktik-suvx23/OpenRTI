/*
Implement the constructor and destructor.
Implement methods for handling synchronization points, discovering object instances, and reflecting attribute values.
Implement methods for getting and setting the federate name and sync label.
Implement the logic for processing received attributes and updating robot attributes.
*/
// #include "RobotFederateAmbassador.h"

#include "RobotFederateAmbassador.h"

MyFederateAmbassador::MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador) {}
    //: _rtiAmbassador(rtiAmbassador), _expectedShipName(expectedShipName), federateName(instance) {}

MyFederateAmbassador::~MyFederateAmbassador() {}

void MyFederateAmbassador::announceSynchronizationPoint(
    std::wstring const& label,
    rti1516e::VariableLengthData const& theUserSuppliedTag)
{
    if (label == L"InitialSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
        syncLabel = label;
    }
    if (label == L"ShutdownSync") {
        std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
        syncLabel = label;
    }
}

void MyFederateAmbassador::discoverObjectInstance(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::ObjectClassHandle theObjectClass,
    std::wstring const &theObjectName)
{
    std::wcout << L"Instance " << federateName << L": Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
    if (theObjectClass == shipClassHandle)
    {
        _shipInstances[theObject] = theObjectClass;
    }
}

void MyFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleValueMap const &theAttributes,
    rti1516e::VariableLengthData const &theTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::SupplementalReflectInfo theReflectInfo)
{
    std::wcout << L"[DEBUG] reflectAttributeValues 1" << std::endl;
    // Extract the ship ID from the attributes into tempShipID
    if (_targetShipID == L"") {
        return;
    }
    std::wcout << L"[DEBUG] reflectAttributeValues 2" << std::endl;
    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
    std::wstring tempShipID;

    if (itShipFederateName != theAttributes.end()) {
        rti1516e::HLAunicodeString attributeValueFederateName;
        attributeValueFederateName.decode(itShipFederateName->second);
        tempShipID = attributeValueFederateName.get();
        std::wcout << L"[DEBUG] Instance " << federateName << L": Received Ship Federate Name: " << tempShipID << L", looking for: " << _targetShipID << std::endl;
        // Only process the update if the tempShipID matches the target ship for the robot
        if (tempShipID != _targetShipID) {
            return; // Skip this update if it's not the target ship
        } else {
            std::wcout << L"Instance " << federateName << L": Update from federate: " << tempShipID << std::endl;
        }
    }

    // Check if the ship is in the list of ship instances
    if (_shipInstances.find(theObject) != _shipInstances.end()) {
        // Handle ship attributes
        auto itShipTag = theAttributes.find(attributeHandleShipTag);
        auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
        auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
        auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);

        // Process the Ship Tag
        if (itShipTag != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueShipTag;
            attributeValueShipTag.decode(itShipTag->second);
            std::wcout << L"Instance " << federateName << L": Received Ship Tag: " << attributeValueShipTag.get() << std::endl;
            tempShipID = attributeValueShipTag.get(); // Update tempShipID for target tracking
            if (tempShipID != _targetShipID) {
                return; // Skip this update if it's not the target ship
            }
        }

        // Process the Ship Position
        if (itShipPosition != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueShipPosition;
            attributeValueShipPosition.decode(itShipPosition->second);
            std::wcout << L"Instance " << federateName << L": Received Ship Position: " << attributeValueShipPosition.get() << std::endl;
            shipPosition = attributeValueShipPosition.get();
        }

        // Process the Future Ship Position
        if (itFutureShipPosition != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFutureShipPosition;
            attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
            std::wcout << L"Instance " << federateName << L": Received Future Ship Position: " << attributeValueFutureShipPosition.get() << std::endl;
            expectedShipPosition = attributeValueFutureShipPosition.get();
        }

        // Process the Ship Speed
        if (itShipSpeed != theAttributes.end()) {
            rti1516e::HLAfloat64BE attributeValueShipSpeed;
            attributeValueShipSpeed.decode(itShipSpeed->second);
            std::wcout << L"Instance " << federateName << L": Received Ship Speed: " << attributeValueShipSpeed.get() << std::endl;
        }

        // Perform calculations based on the attributes received (position, speed, etc.)
        try {
            double initialBearing = _robot.calculateInitialBearingWstring(currentPosition, shipPosition);
            currentPosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
            currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
            currentAltitude = _robot.reduceAltitude(currentAltitude, currentSpeed, currentDistance);
            expectedFuturePosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);

            // Debug output for the calculated positions and distances
            std::wcout << std::endl
                       << L"Instance " << federateName << L": Robot Current Position: " << currentPosition << std::endl;
            std::wcout << L"Instance " << federateName << L": Ship Current Position: " << shipPosition << std::endl;
            std::wcout << L"Instance " << federateName << L": Robot Future Position: " << expectedFuturePosition << std::endl;
            std::wcout << L"Instance " << federateName << L": Ship Future Position: " << expectedShipPosition << std::endl;
            std::wcout << L"Instance " << federateName << L": Robot Current Altitude: " << currentAltitude << std::endl;

            // Check distance and update accordingly
            if (currentDistance < 50)
                currentDistance = 10;
            std::wcout << L"Instance " << federateName << L": Distance between robot and ship: " << currentDistance << " meters" << std::endl;

            if (currentDistance < 1000) {
                std::wcout << L"Instance " << federateName << L": Robot is within 1000 meters of target" << std::endl;
                if (currentDistance < 100) {
                    std::wcout << L"Instance " << federateName << L": Robot is within 100 meters of target" << std::endl;
                    if (currentDistance < 50) {
                        std::wcout << L"Target reached" << std::endl;
                        currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
                        std::wcout << L"Instance " << federateName << L": Distance between robot and ship before last contact: " << currentDistance << " meters" << std::endl;
                        hitStatus = true;
                    }
                }
            }
        } catch (const std::invalid_argument &e) {
            std::wcerr << L"Instance " << federateName << L": Invalid position format" << std::endl;
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
        }
        
        std::wcout << L"[DEBUG] Received Robot ID: " << receivedRobotID << std::endl;
        if (receivedRobotID == L"EMPTY") {
            assignedTarget = true;
        } else if (receivedRobotID != federateName) {
            return;
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