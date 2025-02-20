/*
Implement the constructor and destructor.
Implement methods for handling synchronization points, discovering object instances, and reflecting attribute values.
Implement methods for getting and setting the federate name and sync label.
Implement the logic for processing received attributes and updating robot attributes.
*/
// #include "RobotFederateAmbassador.h"

#include "RobotFederateAmbassador.h"

MyFederateAmbassador::MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, const std::wstring &expectedShipName, int instance)
    : _rtiAmbassador(rtiAmbassador), _expectedShipName(expectedShipName), _instance(instance) {}

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
    std::wcout << L"Instance " << _instance << L": Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
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
    auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);

    if (itShipFederateName != theAttributes.end())
    {
        rti1516e::HLAunicodeString attributeValueFederateName;

        if (itShipFederateName != theAttributes.end())
        {
            attributeValueFederateName.decode(itShipFederateName->second);
        }
        if (attributeValueFederateName.get() != _expectedShipName)
        {
            return;
        }
        else
        {
            std::wcout << L"Instance " << _instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl
                       << std::endl;
        }
    }

    if (_shipInstances.find(theObject) != _shipInstances.end())
    {
        // Handle ship attributes
        auto itShipTag = theAttributes.find(attributeHandleShipTag);
        auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
        auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
        auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
        auto itShipSize = theAttributes.find(attributeHandleShipSize);
        auto itNumberOfRobots = theAttributes.find(attributeHandleNumberOfRobots);

        if (itShipTag != theAttributes.end())
        {
            rti1516e::HLAunicodeString attributeValueShipTag;
            attributeValueShipTag.decode(itShipTag->second);
            std::wcout << L"Instance " << _instance << L": Received Ship Tag: " << attributeValueShipTag.get() << std::endl;
        }
        if (itShipPosition != theAttributes.end())
        {
            rti1516e::HLAunicodeString attributeValueShipPosition;
            attributeValueShipPosition.decode(itShipPosition->second);
            std::wcout << L"Instance " << _instance << L": Received Ship Position: " << attributeValueShipPosition.get() << std::endl;
            shipPosition = attributeValueShipPosition.get();
        }
        if (itFutureShipPosition != theAttributes.end())
        {
            rti1516e::HLAunicodeString attributeValueFutureShipPosition;
            attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
            std::wcout << L"Instance " << _instance << L": Received Future Ship Position: " << attributeValueFutureShipPosition.get() << std::endl;
            expectedShipPosition = attributeValueFutureShipPosition.get();
        }
        if (itShipSpeed != theAttributes.end())
        {
            rti1516e::HLAfloat64BE attributeValueShipSpeed;
            attributeValueShipSpeed.decode(itShipSpeed->second);
            std::wcout << L"Instance " << _instance << L": Received Ship Speed: " << attributeValueShipSpeed.get() << std::endl;
        }
        if (itShipSize != theAttributes.end())
        {
            rti1516e::HLAfloat64BE attributeValueShipSize;
            attributeValueShipSize.decode(itShipSize->second);
            shipSize = attributeValueShipSize.get();
            std::wcout << L"Instance " << _instance << L": Received Ship Size: " << attributeValueShipSize.get() << std::endl;
        }
        if (itNumberOfRobots != theAttributes.end())
        {
            rti1516e::HLAinteger32BE attributeValueNumberOfRobots;
            attributeValueNumberOfRobots.decode(itNumberOfRobots->second);
            numberOfRobots = attributeValueNumberOfRobots.get();
            std::wcout << L"Instance " << _instance << L": Received Number of Robots: " << numberOfRobots << std::endl;
        }

        // Calculate distance and initial bearing between publisher and ship positions
        try
        {
            double initialBearing = _robot.calculateInitialBearingWstring(currentPosition, shipPosition);
            currentPosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
            currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
            currentAltitude = _robot.reduceAltitude(currentAltitude, currentSpeed, currentDistance);
            expectedFuturePosition = _robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
            std::wcout << std::endl
                       << L"Instance " << _instance << L": Robot Current Position: " << currentPosition << std::endl;
            std::wcout << L"Instance " << _instance << L": Ship Current Position: " << shipPosition << std::endl;
            std::wcout << L"Instance " << _instance << L": Robot Future Position: " << expectedFuturePosition << std::endl;
            std::wcout << L"Instance " << _instance << L": Ship Future Position: " << expectedShipPosition << std::endl;
            std::wcout << L"Instance " << _instance << L": Robot Current Altitude: " << currentAltitude << std::endl;

            if (currentDistance < 50)
                currentDistance = 10;
            std::wcout << L"Instance " << _instance << L": Distance between robot and ship: " << currentDistance << " meters" << std::endl;
            if (currentDistance < 1000)
            {
                std::wcout << L"Instance " << _instance << L": Robot is within 1000 meters of target" << std::endl;
                if (currentDistance < 100)
                {
                    std::wcout << L"Instance " << _instance << L": Robot is within 100 meters of target" << std::endl;
                    if (currentDistance < 50)
                    {
                        std::vector<std::wstring> finalData;
                        finalData.push_back(std::to_wstring(_instance));
                        finalData.push_back(std::to_wstring(currentDistance));
                        finalData.push_back(std::to_wstring(currentAltitude));
                        finalData.push_back(std::to_wstring(currentSpeed));
                        finalData.push_back(currentPosition);
                        finalData.push_back(shipPosition);
                        finalData.push_back(expectedShipPosition);
                        finalData.push_back(std::to_wstring(shipSize));
                        finalData.push_back(std::to_wstring(numberOfRobots));
                        
                        // Write the final data to a text file
                        writeDataToFile(finalData, "finalData.txt");
                        std::wcout << L"Target reached" << std::endl;
                        currentDistance = _robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
                        std::wcout << L"Instance " << _instance << L": Distance between robot and ship before last contact: " << currentDistance << " meters" << std::endl;
                        _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
                    }
                }
            }
        }
        catch (const std::invalid_argument &e)
        {
            std::wcerr << L"Instance " << _instance << L": Invalid position format" << std::endl;
        }
    }
}
void writeDataToFile(const std::vector<std::wstring>& data, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto& entry : data) {
            outFile << std::string(entry.begin(), entry.end()) << std::endl;
        }
        outFile.close();
        std::wcout << L"Data successfully written to " << filename.c_str() << std::endl;
    } else {
        std::wcerr << L"Unable to open file: " << filename.c_str() << std::endl;
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