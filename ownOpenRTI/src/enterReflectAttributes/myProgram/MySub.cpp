#include <memory>
#include "RTI/RTIambassadorFactory.h"
#include "RTI/RTIambassador.h"
#include "RTI/NullFederateAmbassador.h"
#include <iostream>
#include <thread>
#include <chrono>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador() {}

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle objectInstanceHandle,
        const rti1516e::AttributeHandleValueMap& attributeValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& time,
        rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle retractionHandle,
        rti1516e::SupplementalReflectInfo reflectInfo) override {
        for (const auto& attribute : attributeValues) {
            if (attribute.first == attributeHandle) {
                uint32_t attributeValue = *reinterpret_cast<const uint32_t*>(attribute.second.data());
                std::wcout << L"Received Attribute1 Value: " << attributeValue << std::endl;
            }
        }
    }

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& time,
        rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle retractionHandle,
        rti1516e::SupplementalReceiveInfo receiveInfo) override {
        if (interactionClassHandle == interactionClassHandle1) {
            auto parameterHandle = parameterValues.find(parameterHandle1);
            if (parameterHandle != parameterValues.end()) {
                uint32_t parameterValue = *reinterpret_cast<const uint32_t*>(parameterHandle->second.data());
                std::wcout << L"Received InteractionClass1 with Parameter1: " << parameterValue << std::endl;
            }
        }
    }

    rti1516e::AttributeHandle attributeHandle;
    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;
};

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>();
        std::wcout << L"Subscriber connecting to RTI using rti protocol..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"AviationSimNet";
        std::wstring federateName = L"SubscriberFederate";
        std::vector<std::wstring> fomModules = {
            L"/usr/OpenRTITest/OpenRTI/src/myProgram/foms/test_fdd.xml"
        };
        std::wstring mimModule = L"/usr/OpenRTITest/OpenRTI/src/myProgram/foms/MIM.xml";
        try {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Subscriber federate joined: " << federateName << std::endl;

        // Get handles and subscribe to attributes and interactions
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ObjectClass1");
        federateAmbassador->attributeHandle = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Attribute1");
        rtiAmbassador->subscribeObjectClassAttributes(objectClassHandle, {federateAmbassador->attributeHandle});
        std::wcout << L"Subscribed to ObjectClass1 and Attribute1" << std::endl;

        federateAmbassador->interactionClassHandle1 = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        federateAmbassador->parameterHandle1 = rtiAmbassador->getParameterHandle(federateAmbassador->interactionClassHandle1, L"Parameter1");
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->interactionClassHandle1);
        std::wcout << L"Subscribed to InteractionClass1" << std::endl;

        // Main loop to keep the federate running and process callbacks
        while (true) {
            rtiAmbassador->evokeCallback(1.0); // Process callbacks with a timeout of 1 second
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}