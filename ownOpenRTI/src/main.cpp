#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_unique<MyFederateAmbassador>();
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"PublisherFederate";
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/foms/FOM.xml");
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Federate joined: " << federateName << std::endl;

        // Get handles and publish attributes
        auto vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        auto positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        auto speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {positionHandle, speedHandle});
        std::wcout << L"Published Vehicle object class and attributes" << std::endl;

        // Register object instance
        auto vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);
        std::wcout << L"Registered Vehicle instance with handle: " << vehicleHandle << std::endl;

        // Update attributes in a loop
        double currentPositionValue = 0.0;
        double currentSpeedValue = 0.0;
        while (true) {
            currentPositionValue += 1.0;
            currentSpeedValue += 0.5;
            rti1516e::AttributeHandleValueMap attributes;
            attributes[positionHandle] = rti1516e::VariableLengthData(reinterpret_cast<const char*>(&currentPositionValue), sizeof(currentPositionValue));
            attributes[speedHandle] = rti1516e::VariableLengthData(reinterpret_cast<const char*>(&currentSpeedValue), sizeof(currentSpeedValue));
            rtiAmbassador->updateAttributeValues(vehicleHandle, attributes, rti1516e::VariableLengthData());
            std::wcout << L"Published Position: " << currentPositionValue << L", Speed: " << currentSpeedValue << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}