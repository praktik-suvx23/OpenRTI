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
        std::wstring federateName = L"SubscriberFederate";
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/foms/FOM.xml");
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Federate joined: " << federateName << std::endl;

        // Get handles and subscribe to attributes
        auto vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        federateAmbassador->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        federateAmbassador->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, {federateAmbassador->positionHandle, federateAmbassador->speedHandle});
        std::wcout << L"Subscribed to attributes" << std::endl;

        // Main loop to evoke multiple callbacks and print current values
        while (true) {
            std::wcout << L"Evoking callbacks..." << std::endl;
            try {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            } catch (const rti1516e::Exception& e) {
                std::wcerr << L"Failed to evoke callbacks: " << e.what() << std::endl;
            }

            {
                std::unique_lock<std::mutex> lock(federateAmbassador->mutex);
                std::wcout << L"Waiting for values to be updated..." << std::endl;
                federateAmbassador->cv.wait(lock, [&] { return federateAmbassador->valuesUpdated; });
                federateAmbassador->valuesUpdated = false;
                std::wcout << L"Current Position: " << federateAmbassador->currentPositionValue << L", Current Speed: " << federateAmbassador->currentSpeedValue << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Adjust the delay as needed
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}