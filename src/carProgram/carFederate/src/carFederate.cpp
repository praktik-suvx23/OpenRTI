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
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

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

    std::cout << "Registering car object..." << std::endl;
    registerCarObject();

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
    fomModule = L"foms/FOM.xml";
    mimModule = L"foms/MIM.xml";
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
        fedAmb->carObjectHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Car");

        // Attribute handles
        fedAmb->carNameHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectHandle, L"Name");
        fedAmb->licensePlateHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectHandle, L"LicensePlateNumber");
        fedAmb->fuelLevelHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectHandle, L"FuelLevel");
        fedAmb->fuelTypeHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectHandle, L"FuelType");
        fedAmb->positionHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectHandle, L"Position");

        // Publish attributes
        rtiAmbassador->publishObjectClassAttributes(fedAmb->carObjectHandle, {
            fedAmb->fuelLevelHandle, 
            fedAmb->positionHandle
        });

        // Subscribe to static attributes
        rtiAmbassador->subscribeObjectClassAttributes(fedAmb->carObjectHandle, {
            fedAmb->carNameHandle,
            fedAmb->licensePlateHandle,
            fedAmb->fuelTypeHandle
        });

        // Interaction handles
        fedAmb->loadScenarioHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.LoadScenario");
        fedAmb->startHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.Start");
        fedAmb->stopHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.Stop");

        // Subscribe to interactions
        rtiAmbassador->subscribeInteractionClass(fedAmb->loadScenarioHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->startHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->stopHandle);

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error initializing handles: " << e.what() << std::endl;
    }
}

void carFederate::loadCarConfig(std::string filePath){
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open car config file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (line.find("Name=") != std::string::npos) {
            fedAmb->carName = line.substr(5);
        } else if (line.find("LicensePlate=") != std::string::npos) {
            fedAmb->licensePlate = line.substr(13);
        } else if (line.find("FuelType=") != std::string::npos) {
            fedAmb->fuelType = line.substr(9);
        } else if (line.find("LitersPer100km1=") != std::string::npos) {
            fedAmb->fuelConsumption1 = std::stod(line.substr(17));
        } else if (line.find("LitersPer100km2=") != std::string::npos) {
            fedAmb->fuelConsumption2 = std::stod(line.substr(17));
        } else if (line.find("LitersPer100km3=") != std::string::npos) {
            fedAmb->fuelConsumption3 = std::stod(line.substr(17));
        }
    }
    configFile.close();
}

void carFederate::registerCarObject() {
    try {
        fedAmb->carObjectInstance = rtiAmbassador->registerObjectInstance(fedAmb->carObjectHandle);

        std::cout << "Registered Car Object with handle: " << fedAmb->carObjectInstance << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error registering car object: " << e.what() << std::endl;
    }
}

void carFederate::run() {
    double timer = 0.0;  // Track elapsed time
    const double timeStep = 1.0;  // Define time step (e.g., 1 second)

    std::cout << "Car federate running..." << std::endl;

    while (true) {
        try {
            // Process RTI callbacks (handle interactions, attribute updates, etc.)
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);

            // Simulate car movement and fuel consumption
            updateCarState();

            // Check if car has reached its goal
            if (hasReachedGoal()) {
                std::cout << "Car reached goal! Time taken: " << timer << " seconds." << std::endl;
                break;  // Exit loop when goal is reached
            }

            // Increment time
            timer += timeStep;

            // Simulate real-time progression (prevent high CPU usage)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

        } catch (const rti1516e::Exception& e) {
            std::cerr << "Error in run loop: " << e.what() << std::endl;
            break;  // Exit on error
        }
    }

    // Final update to publish final car state
    updateCarState();
}

void carFederate::updateCarState(){
    try {
        rti1516e::AttributeHandleValueMap attributes;

        rti1516e::HLAfloat64BE hlaFuelLevel(fedAmb->fuelLevel);
        rti1516e::HLAfloat64BE hlaPositionLat(fedAmb->position.latitude);
        rti1516e::HLAfloat64BE hlaPositionLong(fedAmb->position.longitude);

        attributes[fedAmb->fuelLevelHandle] = hlaFuelLevel.encode();
        attributes[fedAmb->positionLatHandle] = hlaPositionLat.encode();
        attributes[fedAmb->positionLongHandle] = hlaPositionLong.encode();

        rtiAmbassador->updateAttributeValues(fedAmb->carObjectInstance, attributes, rti1516e::VariableLengthData());

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error updating car state: " << e.what() << std::endl;
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