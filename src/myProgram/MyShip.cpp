#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTime.h>
#include <RTI/VariableLengthData.h>
#include <RTI/Handle.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <random>

class MyPublisherFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyPublisherFederateAmbassador() {}
};

void startShip(int instance) {
    std::wstring federateName = L"ShipPublisher" + std::to_wstring(instance);

    try {
        // Create and connect RTIambassador
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyPublisherFederateAmbassador>();
        std::wcout << L"Publisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
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
        std::wcout << L"MyShip joined: " << federateName << std::endl;

        // Get handles and register object instance
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        auto attributeHandleShipTag = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Ship-tag");
        auto attributeHandlePosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Position");
        auto attributeHandleSpeed = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Speed");
        auto attributeHandleFederateName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(attributeHandleShipTag);
        attributes.insert(attributeHandlePosition);
        attributes.insert(attributeHandleSpeed);
        attributes.insert(attributeHandleFederateName);
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, attributes);
        std::wcout << L"Published ship with attributes" << std::endl;

        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << objectInstanceHandle << std::endl;

        // Random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 100);

        // Main loop to update attributes
        while (true) {
            // Process callbacks
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl;

            // Update attributes
            rti1516e::HLAunicodeString attributeValueShipTag(L"Ship" + std::to_wstring(instance));
            rti1516e::HLAunicodeString attributeValuePosition(L"37.7749,-122.4194"); // Example position as a string
            rti1516e::HLAfloat64BE attributeValueSpeed(dis(gen));
            rti1516e::HLAunicodeString attributeValueFederateName(federateName);

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleShipTag] = attributeValueShipTag.encode();
            attributeValues[attributeHandlePosition] = attributeValuePosition.encode();
            attributeValues[attributeHandleSpeed] = attributeValueSpeed.encode();
            attributeValues[attributeHandleFederateName] = attributeValueFederateName.encode();

            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
            std::wcout << L"Updated attributes for ship" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 3; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startShip, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}