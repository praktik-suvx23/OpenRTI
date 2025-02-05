#include "RTI/RTIambassadorFactory.h"
#include "RTI/RTIambassador.h"
#include "RTI/NullFederateAmbassador.h"
#include "RTI/encoding/BasicDataElements.h"
#include "RTI/encoding/EncodingExceptions.h"
#include "RTI/encoding/DataElement.h"
#include "MyPositionDecoder.h"
#include "MyFloat32Decoder.h"
#include "ObjectInstanceHandleHash.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador)
        : _rtiAmbassador(rtiAmbassador) {}

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override {
        std::wcout << L"receiveInteraction called" << std::endl;
        if (interactionClassHandle == interactionClassHandle1) {
            auto parameterHandle = parameterValues.find(parameterHandle1);
            if (parameterHandle != parameterValues.end()) {
                rti1516e::HLAinteger32BE parameterData;
                parameterData.decode(parameterHandle->second);
                std::wcout << L"Received InteractionClass1 with Parameter1: " << parameterData.get() << std::endl;
            }
        }
    }

    rti1516e::InteractionClassHandle interactionClassHandle1;
    rti1516e::ParameterHandle parameterHandle1;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>(rtiAmbassador.get());
        std::wcout << L"Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"AviationSimNet";
        std::wstring federateName = L"MyFederate";
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
        std::wcout << L"MyFederate joined: " << federateName << std::endl;

        // Get handles and subscribe to interactions
        federateAmbassador->interactionClassHandle1 = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        federateAmbassador->parameterHandle1 = rtiAmbassador->getParameterHandle(federateAmbassador->interactionClassHandle1, L"Parameter1");
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->interactionClassHandle1);
        std::wcout << L"Subscribed to InteractionClass1" << std::endl;

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

    return 0;
}