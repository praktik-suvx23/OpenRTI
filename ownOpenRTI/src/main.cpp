/*===================================================================================
    # Code is somewhat working. It atleast is able to load the FOM.xml file. 
    # Unsure if it is able to publish its data to a subscriber.
    # But hey, atleast some progress!
=====================================================================================*/
#include <memory>
#include "MyFederateAmbassador.h"
#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <iostream>
#include <string>
#include <codecvt>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <locale>

void openFileCheck(const std::string& fomFilePath) {
    std::cout << "FOM file path: " << fomFilePath << std::endl;

    std::ifstream file(fomFilePath);

    if (file.is_open()) {
        std::cout << "FOM.xml file successfully opened!" << std::endl;
    } else {
        std::cerr << "Failed to open FOM.xml file." << std::endl;
    }
}

void updateAttributes(rti1516e::RTIambassador* rtiAmbassador, rti1516e::ObjectInstanceHandle vehicleHandle, 
                      rti1516e::AttributeHandle positionHandle, rti1516e::AttributeHandle speedHandle, 
                      double positionValue, double speedValue) {
    std::cout << "Updating Vehicle attributes..." << std::endl;
    rti1516e::VariableLengthData tag;
    rti1516e::AttributeHandleValueMap attributes;

    rti1516e::VariableLengthData positionData(reinterpret_cast<const void*>(&positionValue), sizeof(positionValue));
    attributes[positionHandle] = positionData;

    rti1516e::VariableLengthData speedData(reinterpret_cast<const void*>(&speedValue), sizeof(speedValue));
    attributes[speedHandle] = speedData;

    rtiAmbassador->updateAttributeValues(vehicleHandle, attributes, tag);

    std::cout << "Updated Position Value: " << positionValue << std::endl;
    std::cout << "Updated Speed Value: " << speedValue << std::endl;
}

bool getCurrentAttributeValues(rti1516e::RTIambassador* rtiAmbassador, MyFederateAmbassador& ambassador, 
                               rti1516e::ObjectInstanceHandle vehicleHandle, rti1516e::AttributeHandle positionHandle, 
                               rti1516e::AttributeHandle speedHandle, double& currentPositionValue, double& currentSpeedValue) {
    std::unique_lock<std::mutex> lock(ambassador.mutex);
    if (ambassador.cv.wait_for(lock, std::chrono::seconds(2), [&ambassador] { return ambassador.valuesUpdated; })) {
        currentPositionValue = ambassador.currentPositionValue;
        currentSpeedValue = ambassador.currentSpeedValue;
        ambassador.valuesUpdated = false;
        return true;
    } else {
        std::cerr << "Timeout waiting for attribute updates." << std::endl;
        return false;
    }
}

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
        std::wstring federateName = L"MyFederate";

        // Create the federation if it does not exist
        try {
            rtiAmbassador->createFederationExecution(federationName, L"/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/FOM.xml");
            std::cout << "Federation created: " << wstringToString(federationName) << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::cout << "Federation already exists: " << wstringToString(federationName) << std::endl;
        }

        rtiAmbassador->joinFederationExecution(federateName, federationName);

        std::string fomFilePath = "/usr/OjOpenRTI/OpenRTI/ownOpenRTI/build/FOM.xml";
        openFileCheck(fomFilePath);

        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        federateAmbassador->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        federateAmbassador->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        // Publish the attributes instead of subscribing to them
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {federateAmbassador->positionHandle, federateAmbassador->speedHandle});

        rti1516e::ObjectInstanceHandle vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);

        double currentPositionValue = 0.0;
        double currentSpeedValue = 0.0;
        bool valuesInitialized = getCurrentAttributeValues(rtiAmbassador.get(), *federateAmbassador, vehicleHandle, 
                                                           federateAmbassador->positionHandle, federateAmbassador->speedHandle, 
                                                           currentPositionValue, currentSpeedValue);

        if (!valuesInitialized) {
            double initialPositionValue = 0.0;
            double initialSpeedValue = 0.0;
            updateAttributes(rtiAmbassador.get(), vehicleHandle, federateAmbassador->positionHandle, 
                             federateAmbassador->speedHandle, initialPositionValue, initialSpeedValue);
            currentPositionValue = initialPositionValue;
            currentSpeedValue = initialSpeedValue;
        }

        while (true) {
            if (getCurrentAttributeValues(rtiAmbassador.get(), *federateAmbassador, vehicleHandle, 
                                          federateAmbassador->positionHandle, federateAmbassador->speedHandle, 
                                          currentPositionValue, currentSpeedValue)) {
                double newPositionValue = 10.0 + currentPositionValue;
                double newSpeedValue = 20.0 + currentSpeedValue;
                updateAttributes(rtiAmbassador.get(), vehicleHandle, federateAmbassador->positionHandle, 
                                 federateAmbassador->speedHandle, newPositionValue, newSpeedValue);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << wstringToString(e.what()) << std::endl;
    }

    return 0;
}