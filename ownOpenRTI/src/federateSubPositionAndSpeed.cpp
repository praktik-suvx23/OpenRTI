#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

class SubscriberFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    void reflectAttributeValues(rti1516e::ObjectInstanceHandle objectInstanceHandle,
                                const rti1516e::AttributeHandleValueMap& attributeValues,
                                const rti1516e::VariableLengthData& tag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType transportationType,
                                const rti1516e::LogicalTime& time,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle retractionHandle,
                                rti1516e::SupplementalReflectInfo reflectInfo) override {
        for (const auto& attribute : attributeValues) {
            if (attribute.first == positionHandle) {
                double positionValue;
                std::memcpy(&positionValue, attribute.second.data(), sizeof(positionValue));
                std::cout << "Updated Position Value: " << positionValue << std::endl;
            } else if (attribute.first == speedHandle) {
                double speedValue;
                std::memcpy(&speedValue, attribute.second.data(), sizeof(speedValue));
                std::cout << "Updated Speed Value: " << speedValue << std::endl;
            }
        }
    }

    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;
};

int main(int argc, char* argv[]) {
    try {
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();

        SubscriberFederateAmbassador ambassador;

        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"SubscriberFederate";
        std::wstring fromFomFile = L"../src/FOM.xml";

        try {
            rtiAmbassador->createFederationExecution(federationName, fromFomFile);
            std::wcout << L"Federation created successfully." << std::endl;
        } catch (rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists." << std::endl;
        }

        // Join federation
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Joined federation." << std::endl;

        // Get handles for Vehicle class and its attributes
        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        ambassador.positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        ambassador.speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        // Subscribe to the Vehicle class and its attributes
        rti1516e::AttributeHandleSet attributeHandles;
        attributeHandles.insert(ambassador.positionHandle);
        attributeHandles.insert(ambassador.speedHandle);
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, attributeHandles);
        std::wcout << L"Subscribed to Vehicle class and its attributes." << std::endl;

        // Main loop to keep the federate running and processing callbacks
        while (true) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        // Resign from federation
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;

        // Destroy federation
        try {
            rtiAmbassador->destroyFederationExecution(federationName);
            std::wcout << L"Federation destroyed successfully." << std::endl;
        } catch (rti1516e::FederationExecutionDoesNotExist&) {
            std::wcout << L"Federation does not exist." << std::endl;
        }

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
    }

    return 0;
}