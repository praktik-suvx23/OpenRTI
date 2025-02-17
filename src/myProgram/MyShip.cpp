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
#include <sstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <vector>
#include <memory>
#include <random>
#include <iomanip>

// Function declarations
double getSpeed();  //not implemented
double getAngle(double currentDirection, double maxTurnRate);  
std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter);
double toRadians(double degrees);
double toDegrees(double radians);
std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing);

class MyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyShipFederateAmbassador() {}

    std::wstring syncLabel = L"";
    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag)
    {
        if (label == L"InitialSync") {
            std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
            syncLabel = label;
        }
    
        // Not in use
        if (label == L"ShutdownSync") {
            std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
            syncLabel = label; 
        }
    }
};

std::wstring generateShipPosition(double publisherLat, double publisherLon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disLat(-0.060000, 0.060000); // Approx. 6500 meters in latitude
    std::uniform_real_distribution<> disLon(-0.060000, 0.060000); // Approx. 6500 meters in longitude

    double shipLat, shipLon;

    shipLat = publisherLat + disLat(gen);
    shipLon = publisherLon + disLon(gen);
  
    std::wstringstream wss;
    wss << shipLat << L"," << shipLon;
    return wss.str();
}

std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter) {
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert degrees to radians
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

double getAngle(double currentDirection, double maxTurnRate) {

    currentDirection = toRadians(currentDirection);
    // Calculate desired change in direction
    double desiredChange = 10.0 * sin(currentDirection * M_PI / 180); // Increase angle
    
    // Limit the change in direction to the maximum turn rate
    if (desiredChange > maxTurnRate) {
        desiredChange = maxTurnRate;
    } else if (desiredChange < -maxTurnRate) {
        desiredChange = -maxTurnRate;
    }
    
    currentDirection += toRadians(desiredChange);
    currentDirection = fmod(toDegrees(currentDirection) + 360, 360);
    return currentDirection;
}

std::wstring updateShipPosition(const std::wstring& position, double speed, double bearing) {
    std::vector<std::wstring> tokens = split(position, L',');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid position format");
    }

    double lat = std::stod(tokens[0]);
    double lon = std::stod(tokens[1]);

    const double R = 6371000; // Radius of the Earth in meters

    double distance = speed * 0.1; // Distance traveled in meters (since speed is in m/s and time is 1 second)
    bearing = 5.0;
    double bearingRad = toRadians(bearing); // Convert bearing to radians

    double latRad = toRadians(lat);
    double lonRad = toRadians(lon);

    double newLat = asin(sin(latRad) * cos(distance / R) + cos(latRad) * sin(distance / R) * cos(bearingRad));
    double newLon = lonRad + atan2(sin(bearingRad) * sin(distance / R) * cos(latRad), cos(distance / R) - sin(latRad) * sin(newLat));

    // Convert from radians to degrees
    newLat = toDegrees(newLat);
    newLon = toDegrees(newLon);

    std::wstringstream wss;
    wss << std::fixed << std::setprecision(8) << newLat << L"," << std::fixed << std::setprecision(8) << newLon;
    return wss.str();
}

void startShipPublisher(int instance) {
    std::wstring federateName = L"ShipPublisher" + std::to_wstring(instance);

    try {
        // Create and connect RTIambassador
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyShipFederateAmbassador>();
        std::wcout << L"ShipPublisher federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
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
        std::wcout << L"MyShip joined: " << federateName << std::endl;

        std::wcout << L"ShipPublisher waiting for synchronization announcement..." << std::endl;
        while(federateAmbassador->syncLabel != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"ShipPublisher received sync point." << std::endl;

        // Get handles and register object instance
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        auto attributeHandleShipTag = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Ship-tag");
        auto attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Position");
        auto attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FuturePosition");
        auto attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Speed");
        auto attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(attributeHandleShipTag);
        attributes.insert(attributeHandleShipPosition);
        attributes.insert(attributeHandleFutureShipPosition);
        attributes.insert(attributeHandleShipSpeed);
        attributes.insert(attributeHandleShipFederateName);
        rtiAmbassador->publishObjectClassAttributes(objectClassHandle, attributes);
        std::wcout << L"Published ship with attributes" << std::endl;

        auto objectInstanceHandle = rtiAmbassador->registerObjectInstance(objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << objectInstanceHandle << std::endl;

        // Random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(10.0, 25.0);
        std::uniform_real_distribution<> myDir(-5.0, 5.00);
        double publisherLat = 20.43829000; //for now check value in MyPublisher.cpp
        double publisherLon = 15.62534000; //for now check value in MyPublisher.cpp
        std::wstring myShipLocation = generateShipPosition(publisherLat, publisherLon);
        std::wstring futureExpectedPosition;
        bool firstPosition = true;  
        double currentDirection = myDir(gen); // Used for updateShipPosition function
        double currentSpeed = 0.0; // Used for updateShipPosition function
        double maxTurnRate = 5.0; // Maximum turn rate in degrees per tick

        // Main loop to update attributes
        while (true) {
            currentSpeed = dis(gen);
            currentDirection = myDir(gen);
            currentDirection = getAngle(currentDirection, maxTurnRate);
            myShipLocation = updateShipPosition(myShipLocation, currentSpeed, currentDirection);
            futureExpectedPosition = updateShipPosition(myShipLocation, currentSpeed, currentDirection);

            // Update attributes
            rti1516e::HLAunicodeString attributeValueShipTag(L"Ship" + std::to_wstring(instance));
            rti1516e::HLAunicodeString attributeValueShipPosition(myShipLocation);
            rti1516e::HLAunicodeString attributeValueFutureShipPosition(futureExpectedPosition);
            rti1516e::HLAfloat64BE attributeValueShipSpeed(currentSpeed);
            rti1516e::HLAunicodeString attributeValueShipFederateName(federateName);

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleShipTag] = attributeValueShipTag.encode(); 
            attributeValues[attributeHandleShipPosition] = attributeValueShipPosition.encode();
            attributeValues[attributeHandleFutureShipPosition] = attributeValueFutureShipPosition.encode();
            attributeValues[attributeHandleShipSpeed] = attributeValueShipSpeed.encode();
            attributeValues[attributeHandleShipFederateName] = attributeValueShipFederateName.encode();

            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributeValues, rti1516e::VariableLengthData());
            std::wcout << L"Updated attributes for ship" << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 100; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startShipPublisher, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    return 0;
}