/*===================================================================================
    # Code is somewhat working. It atleast is able to load the FOM.xml file. 
    # Unsure if it is able to publish its data to a subscriber.
    # But hey, atleast some progress!
=====================================================================================*/

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
#include <mutex>
#include <condition_variable>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
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
        std::unique_lock<std::mutex> lock(mutex);
        for (const auto& attribute : attributeValues) {
            if (attribute.first == positionHandle) {
                std::memcpy(&currentPositionValue, attribute.second.data(), sizeof(currentPositionValue));
            } else if (attribute.first == speedHandle) {
                std::memcpy(&currentSpeedValue, attribute.second.data(), sizeof(currentSpeedValue));
            }
        }
        valuesUpdated = true;
        cv.notify_all();
    }

    double currentPositionValue = 0.0;
    double currentSpeedValue = 0.0;
    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;
    std::mutex mutex;
    std::condition_variable cv;
    bool valuesUpdated = false;
};

void openFileCheck(const std::string& fomFilePath) {
    std::cout << "FOM file path: " << fomFilePath << std::endl;

    std::ifstream file(fomFilePath);

    if (file.is_open()) {
        std::cout << "FOM.xml file successfully opened!" << std::endl;
    } else {
        std::cout << "Error: Unable to open FOM.xml file at path: " << fomFilePath << std::endl;
    }
}

void updateAttributes(rti1516e::RTIambassador* rtiAmbassador, rti1516e::ObjectInstanceHandle vehicleHandle, rti1516e::AttributeHandle positionHandle, 
                    rti1516e::AttributeHandle speedHandle, double positionValue, double speedValue) {
    std::cout << "Updating Vehicle attributes..." << std::endl;
    // Create an empty VariableLengthData for the tag
    rti1516e::VariableLengthData tag;

    // Set values for the Vehicle attributes
    rti1516e::AttributeHandleValueMap attributes;

    // Update position and speed values
    rti1516e::VariableLengthData positionData(reinterpret_cast<const void*>(&positionValue), sizeof(positionValue));
    attributes[positionHandle] = positionData;

    rti1516e::VariableLengthData speedData(reinterpret_cast<const void*>(&speedValue), sizeof(speedValue));
    attributes[speedHandle] = speedData;

    // Update the attribute values
    rtiAmbassador->updateAttributeValues(vehicleHandle, attributes, tag);

    // Output the updated values
    std::cout << "Updated Position Value: " << positionValue << std::endl;
    std::cout << "Updated Speed Value: " << speedValue << std::endl;
    
}

bool getCurrentAttributeValues(rti1516e::RTIambassador* rtiAmbassador, MyFederateAmbassador& ambassador, rti1516e::ObjectInstanceHandle vehicleHandle, 
        rti1516e::AttributeHandle positionHandle, rti1516e::AttributeHandle speedHandle, double& currentPositionValue, double& currentSpeedValue) {
    // Request attribute value update
    rti1516e::AttributeHandleSet attributeHandles;
    attributeHandles.insert(positionHandle);
    attributeHandles.insert(speedHandle);
    rtiAmbassador->requestAttributeValueUpdate(vehicleHandle, attributeHandles, rti1516e::VariableLengthData());

    // Wait for the attribute values to be reflected with a timeout
    std::unique_lock<std::mutex> lock(ambassador.mutex);
    if (ambassador.cv.wait_for(lock, std::chrono::seconds(2), [&ambassador] { return ambassador.valuesUpdated; })) {
        // Extract the current values
        currentPositionValue = ambassador.currentPositionValue;
        currentSpeedValue = ambassador.currentSpeedValue;
        return true;
    } else {
        // Timeout occurred, values not updated
        return false;
    }
}

int main(int argc, char* argv[]) {
    try {
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();

        MyFederateAmbassador ambassador;

        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"MyFederate";
        std::wstring fromFomFile = L"foms/FOM.xml";
        std::string fomFilePath = "foms/FOM.xml";
        openFileCheck(fomFilePath);

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

        // Publish and subscribe to the Vehicle class and its attributes
        rti1516e::AttributeHandleSet attributeHandles;
        attributeHandles.insert(ambassador.positionHandle);
        attributeHandles.insert(ambassador.speedHandle);
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, attributeHandles);
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, attributeHandles);
        std::wcout << L"Published and subscribed to Vehicle class and its attributes." << std::endl;

        // Register an instance of the Vehicle class
        rti1516e::ObjectInstanceHandle vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);
        std::wcout << L"Vehicle instance registered with handle: " << vehicleHandle << std::endl;

        // Get current values for attributes
        double currentPositionValue = 0.0;
        double currentSpeedValue = 0.0;
        bool valuesInitialized = getCurrentAttributeValues(rtiAmbassador.get(), ambassador, vehicleHandle, ambassador.positionHandle, ambassador.speedHandle, currentPositionValue, currentSpeedValue);

        if (!valuesInitialized) {
            // Set initial values for attributes
            double initialPositionValue = 0.0;
            double initialSpeedValue = 0.0;

            // Update attributes with initial values
            updateAttributes(rtiAmbassador.get(), vehicleHandle, ambassador.positionHandle, ambassador.speedHandle, initialPositionValue, initialSpeedValue);

            // Use initial values for further processing
            currentPositionValue = initialPositionValue;
            currentSpeedValue = initialSpeedValue;
        }

        // Set new values for attributes based on current values
        double newPositionValue = 10.0 + currentPositionValue;
        double newSpeedValue = 20.0 + currentSpeedValue;

        // Update attributes with new values
        updateAttributes(rtiAmbassador.get(), vehicleHandle, ambassador.positionHandle, ambassador.speedHandle, newPositionValue, newSpeedValue);
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, attributeHandles);

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