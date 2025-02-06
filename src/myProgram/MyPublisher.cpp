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

class MyPublisherFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyPublisherFederateAmbassador() {}
};

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyPublisherFederateAmbassador>();
        std::wcout << L"Publisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"AviationSimNet";
        std::wstring federateName = L"MyPublisher";
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

        // Get handles and publish interactions
        auto interactionClassHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        auto parameterHandle = rtiAmbassador->getParameterHandle(interactionClassHandle, L"Parameter1");
        rtiAmbassador->publishInteractionClass(interactionClassHandle);
        std::wcout << L"Published InteractionClass1" << std::endl;

        // Main loop to process callbacks and send interactions
        int32_t receivedValue = 0;
        while (true) {
            // Process callbacks

            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::wcout << L"Processed callbacks" << std::endl;
       
            receivedValue += 1;
            // Send interaction 
            rti1516e::HLAinteger32BE parameterValue(receivedValue);
            rti1516e::ParameterHandleValueMap parameters;
            parameters[parameterHandle] = parameterValue.encode();
            rtiAmbassador->sendInteraction(interactionClassHandle, parameters, rti1516e::VariableLengthData());
            std::wcout << L"Sent InteractionClass1 with Parameter1: " << receivedValue << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}