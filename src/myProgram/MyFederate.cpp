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
        auto itName = theAttributes.find(attributeHandleName);
        auto itTailNumber = theAttributes.find(attributeHandleTailNumber);
        auto itFuelLevel = theAttributes.find(attributeHandleFuelLevel);
        auto itFuelType = theAttributes.find(attributeHandleFuelType);
        auto itPosition = theAttributes.find(attributeHandlePosition);
        auto itAltitude = theAttributes.find(attributeHandleAltitude);
        auto itDistancToTarget = theAttributes.find(attributeHandleDistanceToTarget);

        if (itName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueName;
            attributeValueName.decode(itName->second);
            std::wcout << L"Received Name: " << attributeValueName.get() << std::endl;
        }
        if (itTailNumber != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueTailNumber;
            attributeValueTailNumber.decode(itTailNumber->second);
            std::wcout << L"Received TailNumber: " << attributeValueTailNumber.get() << std::endl;
        }
        if (itFuelLevel != theAttributes.end()) {
            rti1516e::HLAinteger32BE attributeValueFuelLevel;
            attributeValueFuelLevel.decode(itFuelLevel->second);
            std::wcout << L"Received FuelLevel: " << attributeValueFuelLevel.get() << std::endl;
        }
        if (itFuelType != theAttributes.end()) {
            rti1516e::HLAinteger32BE attributeValueFuelType;
            attributeValueFuelType.decode(itFuelType->second);
            std::wcout << L"Received FuelType: " << attributeValueFuelType.get() << std::endl;
        }
        if (itPosition != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValuePosition;
            attributeValuePosition.decode(itPosition->second);
            std::wcout << L"Received Position: " << attributeValuePosition.get() << std::endl;
        }
        if (itAltitude != theAttributes.end()) {
            rti1516e::HLAfloat64BE attributeValueAltitude;
            attributeValueAltitude.decode(itAltitude->second);
            std::wcout << L"Received Altitude: " << attributeValueAltitude.get() << std::endl;
        }
        if (itDistancToTarget != theAttributes.end()) {
            rti1516e::HLAfloat64BE attributeValueDistanceToTarget;
            attributeValueDistanceToTarget.decode(itDistancToTarget->second);
            std::wcout << L"Received Distance to target: " << attributeValueDistanceToTarget.get() << std::endl;
        }
    }

    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandleName;
    rti1516e::AttributeHandle attributeHandleTailNumber;
    rti1516e::AttributeHandle attributeHandleFuelLevel;
    rti1516e::AttributeHandle attributeHandleFuelType;
    rti1516e::AttributeHandle attributeHandlePosition;
    rti1516e::AttributeHandle attributeHandleAltitude;
    rti1516e::AttributeHandle attributeHandleDistanceToTarget;
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
        federateAmbassador->attributeHandleDistanceToTarget = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"DistanceToTarget");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->attributeHandleName);
        attributes.insert(federateAmbassador->attributeHandleTailNumber);
        attributes.insert(federateAmbassador->attributeHandleFuelLevel);
        attributes.insert(federateAmbassador->attributeHandleFuelType);
        attributes.insert(federateAmbassador->attributeHandlePosition);
        attributes.insert(federateAmbassador->attributeHandleAltitude);
        attributes.insert(federateAmbassador->attributeHandleDistanceToTarget);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->objectClassHandle, attributes);
        std::wcout << L"Subscribed to robot attributes" << std::endl;

        // Main loop to process callbacks
        while (true) {
            // Process callbacks
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl;

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