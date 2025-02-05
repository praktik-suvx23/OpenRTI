#include "RTI/RTIambassadorFactory.h"
#include "RTI/RTIambassador.h"
#include "RTI/NullFederateAmbassador.h"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_unique<rti1516e::NullFederateAmbassador>();
        std::wcout << L"Publisher connecting to RTI using rti protocol..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"AviationSimNet";
        std::wstring federateName = L"PublisherFederate";
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
        std::wcout << L"Publisher federate joined: " << federateName << std::endl;

        // Get handles and publish attributes
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ObjectClass1");
        auto attributeHandle = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Attribute1");
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, {attributeHandle});
        std::wcout << L"Published ObjectClass1 and Attribute1" << std::endl;

        // Register object instance
        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectClass1 instance with handle: " << objectInstanceHandle.toString() << std::endl;

        // Get handles for interaction
        auto interactionClassHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        auto parameterHandle = rtiAmbassador->getParameterHandle(interactionClassHandle, L"Parameter1");

        // Publish interaction class
        rtiAmbassador->publishInteractionClass(interactionClassHandle);
        std::wcout << L"Published InteractionClass1" << std::endl;

        // Update attributes and send interaction in a loop
        uint32_t attributeValue = 123456; // Use uint32_t for larger values
        uint32_t parameterValue = 654321; // Use uint32_t for interaction parameter
        while (true) {
            // Update attribute values
            rti1516e::AttributeHandleValueMap attributes;
            attributes[attributeHandle] = rti1516e::VariableLengthData(reinterpret_cast<const char*>(&attributeValue), sizeof(attributeValue));
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
            std::wcout << L"Published Attribute1: " << attributeValue << std::endl;

            // Send interaction
            rti1516e::ParameterHandleValueMap parameters;
            parameters[parameterHandle] = rti1516e::VariableLengthData(reinterpret_cast<const char*>(&parameterValue), sizeof(parameterValue));
            rtiAmbassador->sendInteraction(interactionClassHandle, parameters, rti1516e::VariableLengthData());
            std::wcout << L"Sent InteractionClass1 with Parameter1: " << parameterValue << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}