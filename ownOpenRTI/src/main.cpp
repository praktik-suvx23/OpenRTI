#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
// Logical time factory and logical time might be needed for the main.cpp file
#include <iostream>
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
        // Create RTIambassador and connect
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_unique<MyFederateAmbassador>();
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost");

        // Create or join federation
        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"PublisherFederate";
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/foms/FOM.xml");
            std::cout << "Federation created: " << wstringToString(federationName) << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::cout << "Federation already exists: " << wstringToString(federationName) << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::cout << "Federate joined: " << wstringToString(federateName) << std::endl;

        // Get handles and publish attributes
        auto vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        auto positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        auto speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {positionHandle, speedHandle});
        std::cout << "Published Vehicle object class and attributes" << std::endl;

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
            std::cout << "Published Position: " << currentPositionValue << ", Speed: " << currentSpeedValue << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << wstringToString(e.what()) << std::endl;
    }

    return 0;
}