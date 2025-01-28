#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <iostream>
#include <sstream> // Include for std::wostringstream
#include <cstring>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>

// Helper function to convert wstring to string
std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
}

// Helper function to convert AttributeHandle to wstring
std::wstring attributeHandleToWString(const rti1516e::AttributeHandle& handle) {
    std::wostringstream woss;
    woss << handle;
    return woss.str();
}

int main(int argc, char* argv[]) {
    try {
        // Use the factory method to create an instance of RTIambassador
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        std::unique_ptr<MyFederateAmbassador> federateAmbassador = std::make_unique<MyFederateAmbassador>();

        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost");

        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"SubscriberFederate";

        // Create the federation if it does not exist
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/foms/FOM.xml");
            std::cout << "Federation created: " << wstringToString(federationName) << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::cout << "Federation already exists: " << wstringToString(federationName) << std::endl;
        }

        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::cout << "Federate joined: " << wstringToString(federateName) << std::endl;

        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        std::wcout << L"Vehicle class handle: " << vehicleClassHandle << std::endl;
        federateAmbassador->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        std::wcout << L"Position attribute handle: " << attributeHandleToWString(federateAmbassador->positionHandle) << std::endl;
        federateAmbassador->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        std::wcout << L"Speed attribute handle: " << attributeHandleToWString(federateAmbassador->speedHandle) << std::endl;

        // Subscribe to the attributes
        try {
            std::cout << "Subscribing to attributes..." << std::endl;
            rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, {federateAmbassador->positionHandle, federateAmbassador->speedHandle});
            std::cout << "Subscribed to attributes" << std::endl;
        } catch (const rti1516e::Exception& e) {
            std::cerr << "Failed to subscribe to attributes: " << wstringToString(e.what()) << std::endl;
            return 1;
        }

        // Main loop to evoke multiple callbacks and print current values
        while (true) {
            std::cout << "Evoking multiple callbacks..." << std::endl;
            bool callbacksInvoked = rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::cout << "Callbacks invoked: " << callbacksInvoked << std::endl;

            {
                std::unique_lock<std::mutex> lock(federateAmbassador->mutex);
                if (federateAmbassador->valuesUpdated) {
                    std::cout << "Current Position Value: " << federateAmbassador->currentPositionValue << std::endl;
                    std::cout << "Current Speed Value: " << federateAmbassador->currentSpeedValue << std::endl;
                    federateAmbassador->valuesUpdated = false;
                }
            }

            // Add a delay to slow down the subscriber
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << wstringToString(e.what()) << std::endl;
    }

    return 0;
}