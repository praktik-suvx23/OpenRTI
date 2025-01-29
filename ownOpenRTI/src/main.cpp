#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/LogicalTimeFactory.h> // Include for LogicalTime
#include <RTI/LogicalTime.h> // Include for LogicalTime
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
        std::wstring federateName = L"PublisherFederate";

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

        rti1516e::AttributeHandle positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        if (!positionHandle.isValid()) {
            std::cerr << "Failed to get Position attribute handle" << std::endl;
            return 1;
        }
        std::wcout << L"Position attribute handle: " << attributeHandleToWString(positionHandle) << std::endl;

        rti1516e::AttributeHandle speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
        if (!speedHandle.isValid()) {
            std::cerr << "Failed to get Speed attribute handle" << std::endl;
            return 1;
        }
        std::wcout << L"Speed attribute handle: " << attributeHandleToWString(speedHandle) << std::endl;

        // Publish the Vehicle object class and its attributes
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {positionHandle, speedHandle});
        std::cout << "Published Vehicle object class and attributes" << std::endl;

        rti1516e::ObjectInstanceHandle vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);
        std::wcout << L"Registered Vehicle instance with handle: " << vehicleHandle << std::endl;

        double currentPositionValue = 0.0;
        double currentSpeedValue = 0.0;

        while (true) {
            // Increment the position and speed values
            currentPositionValue += 1.0;
            currentSpeedValue += 0.5;
            std::cout << "Updating attributes..." << std::endl;

            rti1516e::AttributeHandleValueMap attributes;
            rti1516e::VariableLengthData positionData(reinterpret_cast<const char*>(&currentPositionValue), sizeof(currentPositionValue));
            rti1516e::VariableLengthData speedData(reinterpret_cast<const char*>(&currentSpeedValue), sizeof(currentSpeedValue));

            attributes[positionHandle] = positionData;
            attributes[speedHandle] = speedData;

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