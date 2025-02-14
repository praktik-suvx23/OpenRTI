#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTime.h>
#include <RTI/VariableLengthData.h>
#include <RTI/Handle.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <random>
#include <cmath>

// Function declarations
std::wstring getPosition();
double getAltitude();
double getFuelLevel(double speed);
double getSpeed();

class MyPublisherFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyPublisherFederateAmbassador() : subscribedToPosition(true) {}

    bool subscribedToPosition;
};

// Random number generator for simulating sensor data
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(0.0, 100.0); // Speed range: 0 to 100 units

std::wstring getPosition(double& currentLatitude, double& currentLongitude) {
    // Simulate position sensor reading (latitude, longitude)
    currentLatitude = 20.4382900; // Increment latitude
    currentLongitude = 15.6253400; // Increment longitude
    return std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
}

double getAltitude() {
    // Simulate altitude sensor reading with oscillation
    static double altitude = 50.0;
    static bool increasing = true;
    static double angle = 60.0; // Angle in degrees

    if (increasing) {
        altitude += 50.0 * sin(angle * M_PI / 180); // Increase altitude
        if (altitude > 3000.0) {
            altitude = 3000.0;
            increasing = false;
        }
    } 
    return altitude;
}

double getFuelLevel(double speed) {
    // Simulate fuel level sensor reading
    static double fuelLevel = 100.0;
    fuelLevel -= 0.1 * (speed/100); // Decrease fuel level
    if (fuelLevel < 0) {
        fuelLevel = 0;
    }
    return fuelLevel;
}

double getSpeed(double cSpeed, double minSpeed, double maxSpeed) {
    // Update the range of the speed distribution based on the current speed
    speedDis.param(std::uniform_real_distribution<>::param_type(cSpeed - 10.0, cSpeed + 10.0));
    // Generate a new random speed value within the updated range
    double newSpeed = speedDis(gen);
    // Clamp the speed value to ensure it stays within the specified range
    if (newSpeed < minSpeed) {
        newSpeed = minSpeed;
    } else if (newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }
    return newSpeed;
}

void startPublisher(int instance) {
    std::wstring federateName = L"Publisher" + std::to_wstring(instance);

    try {
        // Create and connect RTIambassador
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyPublisherFederateAmbassador>();
        std::wcout << L"Publisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");

        // Create or join federation
        std::wstring federationName = L"robotFederation";
        std::vector<std::wstring> fomModules = {
            L"foms/robot.xml"
        };
        std::wstring mimModule = L"foms/MIM.xml";
        try {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"MyPublisher joined: " << federateName << std::endl;

        // Get handles and register object instance
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.robot");
        auto attributeHandleName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"robot-x");
        auto attributeHandleSpeed = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Speed");
        auto attributeHandleFuelLevel = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FuelLevel");
        auto attributeHandlePosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Position");
        auto attributeHandleAltitude = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Altitude");
        auto attributeHandleFederateName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet attributes;
        
        attributes.insert(attributeHandleName);
        attributes.insert(attributeHandleSpeed);
        attributes.insert(attributeHandleFuelLevel);
        attributes.insert(attributeHandlePosition);
        attributes.insert(attributeHandleAltitude);
        attributes.insert(attributeHandleFederateName);
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, attributes);
        std::wcout << L"Published robot with attributes" << std::endl;

        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << objectInstanceHandle << std::endl;

        // Current values
        double currentSpeed = 0.0;
        double currentFuelLevel = 100.0;
        double currentLatitude = 0.0;
        double currentLongitude = 0.0;
        std::wstring currentPosition = std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
        double currentAltitude = 0.0;

        // Main loop to update attributes
        while (true) {
            currentSpeed = getSpeed(currentSpeed, 250.0, 450.0);
            currentFuelLevel = getFuelLevel(currentSpeed);
            currentPosition = getPosition(currentLatitude, currentLongitude);
            currentAltitude = getAltitude();

            // Update attributes
            rti1516e::HLAunicodeString attributeValueName(L"Robot" + std::to_wstring(instance));
            rti1516e::HLAfloat64BE attributeValueSpeed(currentSpeed);
            rti1516e::HLAfloat64BE attributeValueFuelLevel(currentFuelLevel);
            rti1516e::HLAunicodeString attributeValuePosition(currentPosition);
            rti1516e::HLAfloat64BE attributeValueAltitude(currentAltitude);
            rti1516e::HLAunicodeString attributeValueFederateName(federateName);

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleName] = attributeValueName.encode();
            attributeValues[attributeHandleSpeed] = attributeValueSpeed.encode();
            attributeValues[attributeHandleFuelLevel] = attributeValueFuelLevel.encode();
            attributeValues[attributeHandlePosition] = attributeValuePosition.encode();
            attributeValues[attributeHandleAltitude] = attributeValueAltitude.encode();
            attributeValues[attributeHandleFederateName] = attributeValueFederateName.encode();

            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
            std::wcout << L"Updated attributes for robot" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 5; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startPublisher, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    return 0;
}