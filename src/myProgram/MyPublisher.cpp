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

void startPublisher(int instance) {
    std::wstring federateName = L"Publisher" + std::to_wstring(instance);

    try {
        // Create and connect RTIambassador
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyPublisherFederateAmbassador>();
        std::wcout << L"Publisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"robotFederation";
        std::vector<std::wstring> fomModules = {
            L"foms/robot.xml"
        };
        std::wstring mimModule = L"foms/MIM.xml";
        try {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"MyPublisher joined: " << federateName << std::endl;

        // Get handles and register object instance
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.robot");
        auto attributeHandleName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"robot-x");
        auto attributeHandleSpeed = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Speed");
        auto attributeHandleFuelLevel = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FuelLevel");
        auto attributeHandlePosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Position");
        auto attributeHandleAltitude = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Altitude");
        auto attributeHandleDistanceToTarget = rtiAmbassador->getAttributeHandle(objectClassHandle, L"DistanceToTarget");
        auto attributeHandleFederateName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(attributeHandleName);
        attributes.insert(attributeHandleSpeed);
        attributes.insert(attributeHandleFuelLevel);
        attributes.insert(attributeHandlePosition);
        attributes.insert(attributeHandleAltitude);
        attributes.insert(attributeHandleDistanceToTarget);
        attributes.insert(attributeHandleFederateName);
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, attributes);
        std::wcout << L"Published robot with attributes" << std::endl;

        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << objectInstanceHandle << std::endl;

        // Random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);

        // Main loop to update attributes
        while (true) {
       

            // Update attributes
            rti1516e::HLAunicodeString attributeValueName(L"Robot" + std::to_wstring(instance));
            rti1516e::HLAfloat64BE attributeValueSpeed(dis(gen));
            rti1516e::HLAinteger32BE attributeValueFuelLevel(dis(gen));
            rti1516e::HLAinteger32BE attributeValueFuelType(1); // Assuming 1 for AviationGasoline
            rti1516e::HLAunicodeString attributeValuePosition(L"37.7749,-122.4194"); // Example position as a string
            rti1516e::HLAfloat64BE attributeValueAltitude(dis(gen) * 1000.0);
            rti1516e::HLAfloat64BE attributeValueDistanceToTarget(dis(gen) * 10.0);
            rti1516e::HLAunicodeString attributeValueFederateName(federateName);

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleName] = attributeValueName.encode();
            attributeValues[attributeHandleFuelLevel] = attributeValueFuelLevel.encode();
            attributeValues[attributeHandlePosition] = attributeValuePosition.encode();
            attributeValues[attributeHandleAltitude] = attributeValueAltitude.encode();
            attributeValues[attributeHandleDistanceToTarget] = attributeValueDistanceToTarget.encode();
            attributeValues[attributeHandleFederateName] = attributeValueFederateName.encode();

            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
            std::wcout << L"Updated attributes for robot" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        threads.emplace_back(startPublisher, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    return 0;
}