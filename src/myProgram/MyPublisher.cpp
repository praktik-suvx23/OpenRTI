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
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyPublisherFederateAmbassador>();
        std::wcout << L"Publisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"AviationSimNet";
        std::vector<std::wstring> fomModules = {
            L"foms/test_fdd.xml"
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
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ObjectClass1");
        auto attributeHandle = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Attribute1");
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(attributeHandle);
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, attributes);
        std::wcout << L"Published ObjectClass1 with Attribute1" << std::endl;

        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << objectInstanceHandle << std::endl;

        // Random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);

        // Main loop to update attribute
        while (true) {
            // Process callbacks
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl;

            int32_t randomValue = dis(gen);
            // Update attribute
            rti1516e::HLAinteger32BE attributeValue(randomValue);
            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandle] = attributeValue.encode();
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
            std::wcout << L"Updated Attribute1 with value: " << randomValue << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
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
        threads.emplace_back(startPublisher, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}