#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>

#include "Robot.h"                    //Include the Robot header file for functions
#include "../include/ObjectInstanceHandleHash.h" // Include the custom hash function
// Function declarations

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador
{
public:
    MyFederateAmbassador(rti1516e::RTIambassador *rtiAmbassador, const std::wstring &expectedShipName, int instance)
        : _rtiAmbassador(rtiAmbassador), _expectedShipName(expectedShipName), _instance(instance), robot() {}

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const &theObjectName) override
    {
        std::wcout << L"Instance " << _instance << L": Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
        if (theObjectClass == shipClassHandle)
        {
            _shipInstances[theObject] = theObjectClass;
        }
    }

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const &theAttributes,
        rti1516e::VariableLengthData const &theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::SupplementalReflectInfo theReflectInfo) override
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

            // Calculate distance and initial bearing between publisher and ship positions
            try
            {
                double initialBearing = robot.calculateInitialBearingWstring(currentPosition, shipPosition);
                currentPosition = robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
                currentDistance = robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
                currentAltitude = robot.reduceAltitude(currentAltitude, currentSpeed, currentDistance);
                expectedFuturePosition = robot.calculateNewPosition(currentPosition, currentSpeed, initialBearing);
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
                            std::wcout << L"Target reached" << std::endl;
                            currentDistance = robot.calculateDistance(currentPosition, shipPosition, currentAltitude);
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

    std::wstring syncLabel = L"";
    void announceSynchronizationPoint(
        std::wstring const &label,
        rti1516e::VariableLengthData const &theUserSuppliedTag)
    {
        if (label == L"InitialSync")
        {
            std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
            syncLabel = label;
        }

        // Not in use
        if (label == L"ShutdownSync")
        {
            std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
            syncLabel = label;
        }
    }

    // MyRobot definitions
    rti1516e::AttributeHandle attributeHandleFederateName;
    Robot _robot;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _objectInstances;

    // MyShip definitions
    rti1516e::ObjectClassHandle shipClassHandle;
    rti1516e::AttributeHandle attributeHandleShipTag;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleFutureShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    std::wstring _expectedPublisherName;
    std::wstring _expectedShipName;

    // HitEvent definitions
    rti1516e::InteractionClassHandle hitEventHandle;
    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle hitConfirmedParam;

    // Ex
    std::wstring RobotPosition;
    std::wstring shipPosition;

    bool firstPosition = true;
    bool heightAchieved = false;
    double currentDistance;

    double currentSpeed = 0.0;
    double currentFuelLevel = 100.0;
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    std::wstring currentPosition = std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
    double currentAltitude = 0.0;

    Robot robot;

    std::wstring expectedFuturePosition;
    std::wstring expectedShipPosition;
    int _instance;

private:
    rti1516e::RTIambassador *_rtiAmbassador;
};

void startRobot(int instance)
{
    std::wstring federateName = L"Subscriber" + std::to_wstring(instance);
    std::wstring expectedShipName = L"ShipPublisher" + std::to_wstring(instance); // Expected ship name

    try
    {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>(rtiAmbassador.get(), expectedShipName, instance);
        std::wcout << L"Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321"); // Using the rti protocol, can be switched with other protocols

        std::wstring federationName = L"robotFederation";
        std::vector<std::wstring> fomModules = {// If you want to use more than one FOM module, add them to the vector
                                                L"foms/robot.xml"};
        std::wstring mimModule = L"foms/MIM.xml";

        // Create or join federation
        try
        {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        }
        catch (const rti1516e::FederationExecutionAlreadyExists &)
        {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"MyFederate joined: " << federateName << std::endl;

        // Achieve sync point
        std::wcout << L"MyFederate waiting for synchronization announcement..." << std::endl;
        while (federateAmbassador->syncLabel != L"InitialSync")
        {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"MyFederate received sync point." << std::endl;

        // Get handles and subscribe to object class attributes
        federateAmbassador->shipClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Position");
        federateAmbassador->attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FuturePosition");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet shipAttributes;
        shipAttributes.insert(federateAmbassador->attributeHandleShipTag);
        shipAttributes.insert(federateAmbassador->attributeHandleShipPosition);
        shipAttributes.insert(federateAmbassador->attributeHandleFutureShipPosition);
        shipAttributes.insert(federateAmbassador->attributeHandleShipSpeed);
        shipAttributes.insert(federateAmbassador->attributeHandleShipFederateName);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, shipAttributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
        bool heightAchieved = false;
        // Main loop to process callbacks and update robot attributes
        federateAmbassador->currentPosition = federateAmbassador->robot.getPosition(federateAmbassador->currentLatitude, federateAmbassador->currentLongitude);
        while (true)
        {
            federateAmbassador->currentSpeed = federateAmbassador->robot.getSpeed(federateAmbassador->currentSpeed, 250.0, 450.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            federateAmbassador->currentFuelLevel = federateAmbassador->robot.getFuelLevel(federateAmbassador->currentSpeed);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (!heightAchieved)
            {
                federateAmbassador->currentAltitude = federateAmbassador->robot.getAltitude();
                if (federateAmbassador->currentAltitude >= 1000.0)
                {
                    federateAmbassador->currentAltitude = 1000.0;
                    heightAchieved = true;
                }
            }
            if (heightAchieved)
            {
                federateAmbassador->currentAltitude = federateAmbassador->robot.reduceAltitude(federateAmbassador->currentAltitude, federateAmbassador->currentSpeed, federateAmbassador->currentDistance);
            }

            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    }
    catch (const rti1516e::Exception &e)
    {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}
int main()
{
    int numInstances = 1; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i)
    {
        threads.emplace_back(startRobot, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    for (auto &thread : threads)
    {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}
