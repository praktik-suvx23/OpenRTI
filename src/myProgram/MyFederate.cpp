#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include "include/ObjectInstanceHandleHash.h" // Include the custom hash function

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, const std::wstring& expectedPublisherName, const std::wstring& expectedShipName, int instance)
        : _rtiAmbassador(rtiAmbassador), _expectedPublisherName(expectedPublisherName), _expectedShipName(expectedShipName), _instance(instance) {}

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const& theObjectName) override {
        std::wcout << L"Instance " << _instance << L": Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
        if (theObjectClass == objectClassHandle) {
            _objectInstances[theObject] = theObjectClass;
        } else if (theObjectClass == shipClassHandle) {
            _shipInstances[theObject] = theObjectClass;
        }
    }

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const& theAttributes,
        rti1516e::VariableLengthData const& theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::SupplementalReflectInfo theReflectInfo) override {

        auto itFederateName = theAttributes.find(attributeHandleFederateName);
        if (itFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            attributeValueFederateName.decode(itFederateName->second);
            std::wstring expectedPublisherName = L"Publisher" + std::to_wstring(_instance);
            std::wstring expectedShipName = L"ShipPublisher" + std::to_wstring(_instance);
            if (attributeValueFederateName.get() != expectedPublisherName && attributeValueFederateName.get() != expectedShipName) {
                std::wcout << L"Instance " << _instance << L": Ignored update from federate: " << attributeValueFederateName.get() << std::endl;
                return;
            } else {
                std::wcout << L"Instance " << _instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl;
            }
        }

        if (_objectInstances.find(theObject) != _objectInstances.end()) {
            // Handle robot attributes
            auto itName = theAttributes.find(attributeHandleName);
            auto itTailNumber = theAttributes.find(attributeHandleTailNumber);
            auto itFuelLevel = theAttributes.find(attributeHandleFuelLevel);
            auto itFuelType = theAttributes.find(attributeHandleFuelType);
            auto itPosition = theAttributes.find(attributeHandlePosition);
            auto itAltitude = theAttributes.find(attributeHandleAltitude);
            std::cout << std::endl;

            if (itName != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueName;
                attributeValueName.decode(itName->second);
                std::wcout << L"Instance " << _instance << L": Received Name: " << attributeValueName.get() << std::endl;
            }
            if (itTailNumber != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueTailNumber;
                attributeValueTailNumber.decode(itTailNumber->second);
                std::wcout << L"Instance " << _instance << L": Received TailNumber: " << attributeValueTailNumber.get() << std::endl;
            }
            if (itFuelLevel != theAttributes.end()) {
                rti1516e::HLAinteger32BE attributeValueFuelLevel;
                attributeValueFuelLevel.decode(itFuelLevel->second);
                std::wcout << L"Instance " << _instance << L": Received FuelLevel: " << attributeValueFuelLevel.get() << std::endl;
            }
            if (itFuelType != theAttributes.end()) {
                rti1516e::HLAinteger32BE attributeValueFuelType;
                attributeValueFuelType.decode(itFuelType->second);
                std::wcout << L"Instance " << _instance << L": Received FuelType: " << attributeValueFuelType.get() << std::endl;
            }
            if (itPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValuePosition;
                attributeValuePosition.decode(itPosition->second);
                std::wcout << L"Instance " << _instance << L": Received Position: " << attributeValuePosition.get() << std::endl;
            }
            if (itAltitude != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueAltitude;
                attributeValueAltitude.decode(itAltitude->second);
                std::wcout << L"Instance " << _instance << L": Received Altitude: " << attributeValueAltitude.get() << std::endl;
            }
        } else if (_shipInstances.find(theObject) != _shipInstances.end()) {
            // Handle ship attributes
            auto itShipTag = theAttributes.find(attributeHandleShipTag);
            auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
            auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
            auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
            
            std::cout << std::endl;

            if (itShipTag != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipTag;
                attributeValueShipTag.decode(itShipTag->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Tag: " << attributeValueShipTag.get() << std::endl;
            }
            if (itShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipPosition;
                attributeValueShipPosition.decode(itShipPosition->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Position: " << attributeValueShipPosition.get() << std::endl;
            }
            if (itShipSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSpeed;
                attributeValueShipSpeed.decode(itShipSpeed->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Speed: " << attributeValueShipSpeed.get() << std::endl;
            }
            if (itShipFederateName != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipFederateName;
                attributeValueShipFederateName.decode(itShipFederateName->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Federate Name: " << attributeValueShipFederateName.get() << std::endl;
            }
        }
    }

    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandleName;
    rti1516e::AttributeHandle attributeHandleTailNumber;
    rti1516e::AttributeHandle attributeHandleFuelLevel;
    rti1516e::AttributeHandle attributeHandleFuelType;
    rti1516e::AttributeHandle attributeHandlePosition;
    rti1516e::AttributeHandle attributeHandleAltitude;
    rti1516e::AttributeHandle attributeHandleFederateName;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _objectInstances;

    rti1516e::ObjectClassHandle shipClassHandle;
    rti1516e::AttributeHandle attributeHandleShipTag;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    std::wstring _expectedPublisherName;
    std::wstring _expectedShipName;
    int _instance;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

void startSubscriber(int instance) {
    std::wstring federateName = L"Subscriber" + std::to_wstring(instance);
    std::wstring expectedPublisherName = L"Publisher" + std::to_wstring(instance); // Expected publisher name
    std::wstring expectedShipName = L"ShipPublisher" + std::to_wstring(instance); // Expected ship name

    try {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>(rtiAmbassador.get(), expectedPublisherName, expectedShipName, instance);
        std::wcout << L"Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"robotFederation";
        std::vector<std::wstring> fomModules = {
            L"/usr/OjOpenRTI/OpenRTI/src/myProgram/foms/robot.xml"
        };
        std::wstring mimModule = L"foms/MIM.xml";
        try {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"MyFederate joined: " << federateName << std::endl;

        // Get handles and subscribe to object class attributes
        federateAmbassador->objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.robot");
        federateAmbassador->attributeHandleName = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"robot-x");
        federateAmbassador->attributeHandleTailNumber = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"TailNumber");
        federateAmbassador->attributeHandleFuelLevel = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FuelLevel");
        federateAmbassador->attributeHandleFuelType = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FuelType");
        federateAmbassador->attributeHandlePosition = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Position");
        federateAmbassador->attributeHandleAltitude = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Altitude");
        federateAmbassador->attributeHandleFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FederateName");

        federateAmbassador->shipClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Position");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet robotAttributes;
        robotAttributes.insert(federateAmbassador->attributeHandleName);
        robotAttributes.insert(federateAmbassador->attributeHandleTailNumber);
        robotAttributes.insert(federateAmbassador->attributeHandleFuelLevel);
        robotAttributes.insert(federateAmbassador->attributeHandleFuelType);
        robotAttributes.insert(federateAmbassador->attributeHandlePosition);
        robotAttributes.insert(federateAmbassador->attributeHandleAltitude);
        robotAttributes.insert(federateAmbassador->attributeHandleFederateName);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->objectClassHandle, robotAttributes);
        std::wcout << L"Subscribed to robot attributes" << std::endl;

        rti1516e::AttributeHandleSet shipAttributes;
        shipAttributes.insert(federateAmbassador->attributeHandleShipTag);
        shipAttributes.insert(federateAmbassador->attributeHandleShipPosition);
        shipAttributes.insert(federateAmbassador->attributeHandleShipSpeed);
        shipAttributes.insert(federateAmbassador->attributeHandleShipFederateName);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, shipAttributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;

        // Main loop to process callbacks
        while (true) {
            // Process callbacks
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int numInstances = 3; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startSubscriber, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}