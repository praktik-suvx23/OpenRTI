#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include "include/MyPositionDecoder.h"
#include "include/MyFloat32Decoder.h"
#include "include/ObjectInstanceHandleHash.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador)
        : _rtiAmbassador(rtiAmbassador) {}

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const& theObjectName) override {
        std::wcout << L"Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
        _objectInstances[theObject] = theObjectClass;
    }

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const& theAttributes,
        rti1516e::VariableLengthData const& theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::SupplementalReflectInfo theReflectInfo) override {
        std::wcout << L"Reflect Attribute Values for ObjectInstance: " << theObject << std::endl;
        auto it = theAttributes.find(attributeHandle);
        if (it != theAttributes.end()) {
            rti1516e::HLAinteger32BE attributeValue;
            attributeValue.decode(it->second);
            std::wcout << L"Received Attribute1 with value: " << attributeValue.get() << std::endl;
        }
    }

    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandle;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _objectInstances;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

void startSubscriber(int instance) {
    std::wstring federateName = L"Subscriber" + std::to_wstring(instance);

    try {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>(rtiAmbassador.get());
        std::wcout << L"Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
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
        std::wcout << L"MyFederate joined: " << federateName << std::endl;

        // Get handles and subscribe to object class attributes
        federateAmbassador->objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ObjectClass1");
        federateAmbassador->attributeHandle = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Attribute1");
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->attributeHandle);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->objectClassHandle, attributes);
        std::wcout << L"Subscribed to ObjectClass1 with Attribute1" << std::endl;

        // Main loop to process callbacks
        while (true) {
            // Process callbacks
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl;

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
        threads.emplace_back(startSubscriber, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}