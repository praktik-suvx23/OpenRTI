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
#include <vector>
#include <memory>
#include <random>

// Function declarations
double getSpeed();  //not implemented
double getAngle();  //not implemented

class MyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyShipFederateAmbassador() {}
};

std::wstring generateShipPosition(double publisherLat, double publisherLon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> disLat(-0.06, 0.06); // Approx. 6500 meters in latitude
    std::uniform_real_distribution<> disLon(-0.06, 0.06); // Approx. 6500 meters in longitude

    double shipLat, shipLon;

        shipLat = publisherLat + disLat(gen);
        shipLon = publisherLon + disLon(gen);
  
    std::wstringstream wss;
    wss << shipLat << L"," << shipLon;
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

        // Get handles and register object instance
        auto objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        auto attributeHandleShipTag = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Ship-tag");
        auto attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Position");
        auto attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(objectClassHandle, L"Speed");
        auto attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(objectClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(attributeHandleShipTag);
        attributes.insert(attributeHandleShipPosition);
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
        double publisherLat = 20.43829; //for now check value in MyPublisher.cpp
        double publisherLon = 15.62534; //for now check value in MyPublisher.cpp
        std::wstring randomShipLocation = generateShipPosition(publisherLat, publisherLon);

        // Main loop to update attributes
        while (true) {

            // Update attributes
            rti1516e::HLAunicodeString attributeValueShipTag(L"Ship" + std::to_wstring(instance));
            rti1516e::HLAunicodeString attributeValueShipPosition(randomShipLocation); // Example position as a string
            rti1516e::HLAfloat64BE attributeValueShipSpeed(dis(gen));
            rti1516e::HLAunicodeString attributeValueShipFederateName(federateName);

            rti1516e::AttributeHandleValueMap attributeValues;
            attributeValues[attributeHandleShipTag] = attributeValueShipTag.encode();
            attributeValues[attributeHandleShipPosition] = attributeValueShipPosition.encode();
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
    int numInstances = 3; // Number of instances to start

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