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
#include <RTI/encoding/HLAfixedRecord.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>

#include "../include/carFederate.h"
#include "../include/carFedAmb.h"

carFederate::carFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

carFederate::~carFederate() {
    finalize();
    std::cout << "Last value subscribed on: " << fedAmb->savedData << std::endl;
}

void carFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<carFedAmb>(rtiAmbassador.get());

    std::cout << "Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
    connectToRTI();

    std::cout << "Creating federation..." << std::endl;
    initializeFederation();

    std::cout << "Joining federation..." << std::endl;
    joinFederation(federateName);

    std::cout << "Achieving sync point..." << std::endl;
    achieveSyncPoint();

    std::cout << "Initializing handles..." << std::endl;
    initializeHandles();

    std::cout << "Loading car configuration..." << std::endl;
    loadCarConfig("carConfig.txt");

    //std::cout << "Registering car object..." << std::endl;
    //registerCarObject();

    std::cout << "Running federate..." << std::endl;
    run();
}

void carFederate::connectToRTI() {
    try{
        rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::RTIinternalError& e){
        std::wcout << L"RTI internal error: " << e.what() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception in runFederate: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::wcout << L"Standard Exception in runFederate: " << e.what() << std::endl;
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void carFederate::initializeFederation() {
    federationName = L"exampleFederation";
    fomModule = L"" FOM_MODULE_PATH;
    mimModule = L"" MIM_MODULE_PATH;
    fomModules = {fomModule};
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const rti1516e::CouldNotOpenFDD&) {
        std::wcout << L"Could not open FDD: " << fomModule << std::endl;
    } catch (const rti1516e::ErrorReadingFDD&) {
        std::wcout << L"Error reading FDD: " << fomModule << std::endl;
    } catch (const rti1516e::CouldNotOpenMIM&) {
        std::wcout << L"Could not open MIM: " << mimModule << std::endl;
    } catch (const rti1516e::ErrorReadingMIM&) {
        std::wcout << L"Error reading MIM: " << mimModule << std::endl;
    } catch (const rti1516e::NotConnected&) {
        std::wcout << L"Not connected" << std::endl;
    } catch (const rti1516e::RTIinternalError&) {
        std::wcout << L"RTI internal error" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception in runFederate: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::wcout << L"Standard Exception in runFederate: " << e.what() << std::endl;
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void carFederate::joinFederation(std::wstring federateName) {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
    } catch (const rti1516e::FederateAlreadyExecutionMember&) {
        std::wcout << L"Federate already execution member: " << federateName << std::endl;
    } catch (const rti1516e::FederationExecutionDoesNotExist&) {
        std::wcout << L"Federation execution does not exist: exampleFederation" << std::endl;
    } catch (const rti1516e::SaveInProgress&) {
        std::wcout << L"Save in progress" << std::endl;
    } catch (const rti1516e::RestoreInProgress&) {
        std::wcout << L"Restore in progress" << std::endl;
    } catch (const rti1516e::RTIinternalError&) {
        std::wcout << L"RTI internal error" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception in runFederate: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::wcout << L"Standard Exception in runFederate: " << e.what() << std::endl;
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void carFederate::achieveSyncPoint() {
    std::wcout << L"Publisher Federate waiting for synchronization announcement..." << std::endl;

    // Process callbacks until the sync point is announced
    while (fedAmb->syncLabel != L"InitialSync") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    std::wcout << L"Publisher Federate received sync point. Ready to achieve it." << std::endl;
}

void carFederate::initializeHandles() {
    try {
        // Object class handle
        fedAmb->carObjectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Car");

        // Attribute handles
        fedAmb->carObjectInstanceHandle = rtiAmbassador->registerObjectInstance(fedAmb->carObjectClassHandle);
        fedAmb->carNameHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Name");
        fedAmb->licensePlateHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"LicensePlateNumber");
        fedAmb->fuelLevelHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"FuelLevel");
        fedAmb->fuelTypeHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"FuelType");
        fedAmb->positionHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Position");

        // Publish attributes
        rtiAmbassador->publishObjectClassAttributes(fedAmb->carObjectClassHandle, {
            fedAmb->positionHandle,
            fedAmb->fuelLevelHandle, 
            fedAmb->fuelTypeHandle,
            fedAmb->licensePlateHandle,
            fedAmb->carNameHandle
        });

        // Subscribe to scenario attributes (car will receive these values)
        fedAmb->startLatHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StartLat");
        fedAmb->startLongHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StartLong");
        fedAmb->stopLatHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StopLat");
        fedAmb->stopLongHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StopLong");
        fedAmb->initFuelHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"FuelLevel");

        // Subscribe to scenario interaction (receives start position, goal position, and initial fuel level)
        rtiAmbassador->subscribeInteractionClass(fedAmb->loadScenarioHandle);

        // Subscribe to start & stop interactions
        rtiAmbassador->subscribeInteractionClass(fedAmb->startHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->stopHandle);

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error initializing handles: " << e.what() << std::endl;
    }
}

void carFederate::loadCarConfig(std::string filePath) {
    std::ifstream configFile(CAR_MODULE_PATH);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open car config file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        size_t delimiterPos = line.find("=");
        if (delimiterPos == std::string::npos) continue; // Skip malformed lines

        std::string key = line.substr(0, delimiterPos);
        std::string value = line.substr(delimiterPos + 1);

        if (key == "Name") {
            fedAmb->carName = value;
        } else if (key == "LicensePlate") {
            fedAmb->licensePlate = value;
        } else if (key == "FuelType") {
            fedAmb->fuelType = value;
        } else if (key == "NormalSpeed") {
            fedAmb->normalSpeed = std::stod(value);
        } else if (key == "LitersPer100km1") {
            fedAmb->fuelConsumption1 = std::stod(value);
        } else if (key == "LitersPer100km2") {
            fedAmb->fuelConsumption2 = std::stod(value);
        } else if (key == "LitersPer100km3") {
            fedAmb->fuelConsumption3 = std::stod(value);
        }
    }
    configFile.close();
}
/*
void carFederate::registerCarObject() {
    try {
        // Register the car object instance
        fedAmb->carObjectClassHandle = rtiAmbassador->registerObjectInstance(fedAmb->carObjectClassHandle);
        std::wcout << "Registered Car Object with handle: " << fedAmb->carObjectClassHandle << std::endl;

        // Create an attribute map
        rti1516e::AttributeHandleValueMap attributes;

        // Encode car name
        rti1516e::HLAunicodeString carNameData(fedAmb->carName);
        attributes[fedAmb->carNameHandle] = carNameData.encode();

        // Encode license plate
        rti1516e::HLAunicodeString licensePlateData(fedAmb->licensePlate);
        attributes[fedAmb->licensePlateHandle] = licensePlateData.encode();

        // Encode initial fuel level
        rti1516e::HLAfloat64LE fuelLevelData(fedAmb->fuelLevel);
        attributes[fedAmb->fuelLevelHandle] = fuelLevelData.encode();

        // Encode initial position using startLat and startLong (Fixes Issue #1)
        rti1516e::HLAfixedRecord positionData;
        positionData.appendElement(rti1516e::HLAfloat64LE(fedAmb->startLat));
        positionData.appendElement(rti1516e::HLAfloat64LE(fedAmb->startLong));
        attributes[fedAmb->positionHandle] = positionData.encode();

        // Send attribute update to RTI
        rtiAmbassador->updateAttributeValues(fedAmb->carObjectClassHandle, attributes, rti1516e::VariableLengthData());

        std::cout << "Initial car attributes published successfully." << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error registering car object: " << e.what() << std::endl;
    }
}
*/

void carFederate::run() {
    auto startTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = 0.0;  // in seconds

    while (fedAmb->simulationRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        elapsedTime = std::chrono::duration<double>(currentTime - startTime).count();

        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);

        updateCarState(elapsedTime);

        // Optionally, check if the car has reached the goal or run out of fuel
        if (fedAmb->fuelLevel <= 0 || (fedAmb->startLat == fedAmb->goalLat && fedAmb->startLong == fedAmb->goalLong)) {
            std::cout << "Simulation complete! Car has either run out of fuel or reached the goal." << std::endl;
            fedAmb->simulationRunning = false;  // Stop the simulation if the goal is reached or fuel is empty
        }

        // Add any additional checks or updates to state here as needed

    }
}

void carFederate::updateCarState(double elapsedTimeInSeconds) {
    try {
        // Assuming you have speed in km/h
        double speedInKmPerSecond = fedAmb->normalSpeed / 3600.0;  // Convert to km per second

        // Calculate the distance traveled in the given time
        double distanceTraveled = speedInKmPerSecond * elapsedTimeInSeconds;  // in km
        
        // Update position (simplified: move towards goal)
        double latDiff = fedAmb->goalLat - fedAmb->startLat;
        double longDiff = fedAmb->goalLong - fedAmb->startLong;

        // Normalize the position change (move a fraction of the way to the goal)
        double totalDistance = std::sqrt(latDiff * latDiff + longDiff * longDiff);  // Euclidean distance
        double movementFraction = distanceTraveled / totalDistance;  // Fraction of the total distance

        // Update the car's position based on the movement
        fedAmb->startLat += latDiff * movementFraction;
        fedAmb->startLong += longDiff * movementFraction;

        // Update fuel level based on the distance traveled (simplified model)
        double fuelConsumed = distanceTraveled * fedAmb->fuelConsumption1 / 100.0;  // Assuming consumption in Liters per 100 km
        fedAmb->fuelLevel -= fuelConsumed;

        // Ensure fuel doesn't go below 0
        if (fedAmb->fuelLevel < 0) {
            fedAmb->fuelLevel = 0;
        }

        // Check if the car has reached its goal
        if (totalDistance <= distanceTraveled) {
            std::cout << "Car has reached the goal!" << std::endl;
            // Stop simulation or take any other action
        }

        // Create an attribute map for the updates
        rti1516e::AttributeHandleValueMap attributes;

        // Encode fuel level
        rti1516e::HLAfloat64LE fuelLevelData(fedAmb->fuelLevel);
        attributes[fedAmb->fuelLevelHandle] = fuelLevelData.encode();

        // Encode updated position
        rti1516e::HLAfixedRecord positionData;
        positionData.appendElement(rti1516e::HLAfloat64LE(fedAmb->startLat));
        positionData.appendElement(rti1516e::HLAfloat64LE(fedAmb->startLong));
        attributes[fedAmb->positionHandle] = positionData.encode();

        // Send attribute update to RTI
        rtiAmbassador->updateAttributeValues(fedAmb->carObjectInstanceHandle, attributes, rti1516e::VariableLengthData());

        std::cout << "Updated position: (" << fedAmb->startLat << ", " << fedAmb->startLong 
                  << ") | Fuel level: " << fedAmb->fuelLevel << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating car state: " << e.what() << std::endl;
    }
}


void carFederate::finalize() {
    try {
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->loadScenarioHandle);
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->startHandle);
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->stopHandle);

        std::wcout << L"Unpublished InteractionClass1" << std::endl;
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void carFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}