/*====================================================================================================
    #   Code doesn't work. Was supposed to listen for updates on speed and position.
    #   The issue might very well be in main.cpp. But this file isn't doing what
    #   it was meant to do. Sadly.
======================================================================================================*/
#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <iostream>
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

int main(int argc, char* argv[]) {
    try {
        // Use the factory method to create an instance of RTIambassador
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        std::unique_ptr<MyFederateAmbassador> federateAmbassador = std::make_unique<MyFederateAmbassador>();

        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost");

        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"SubscriberFederate";
        rtiAmbassador->joinFederationExecution(federateName, federationName);

        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        federateAmbassador->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        federateAmbassador->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        // Subscribe to the attributes
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, {federateAmbassador->positionHandle, federateAmbassador->speedHandle});

        // Main loop to evoke callbacks
        while (true) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << wstringToString(e.what()) << std::endl;
    }

    return 0;
}