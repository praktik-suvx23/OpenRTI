#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <iostream>
#include <thread>
#include <chrono>

// Helper function to convert wstring to string
std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

int main(int argc, char* argv[]) {
    try {
        // Create RTIambassador and connect
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_unique<MyFederateAmbassador>();
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost");

        // Create or join federation
        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"SubscriberFederate";
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/foms/FOM.xml");
            std::cout << "Federation created: " << wstringToString(federationName) << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::cout << "Federation already exists: " << wstringToString(federationName) << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::cout << "Federate joined: " << wstringToString(federateName) << std::endl;

        // Get handles and subscribe to attributes
        auto vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        federateAmbassador->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        federateAmbassador->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, {federateAmbassador->positionHandle, federateAmbassador->speedHandle});
        std::cout << "Subscribed to attributes" << std::endl;

        // Main loop to evoke multiple callbacks and print current values
        while (true) {
            try {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            } catch (const rti1516e::Exception& e) {
                std::cerr << "Failed to evoke callbacks: " << wstringToString(e.what()) << std::endl;
            }

            {
                std::unique_lock<std::mutex> lock(federateAmbassador->mutex);
                federateAmbassador->cv.wait(lock, [&] { return federateAmbassador->valuesUpdated; });
                federateAmbassador->valuesUpdated = false;
                std::cout << "Current Position: " << federateAmbassador->currentPositionValue << ", Current Speed: " << federateAmbassador->currentSpeedValue << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Adjust the delay as needed
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << wstringToString(e.what()) << std::endl;
    }

    return 0;
}