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
#include "../include/carMath.h"
#include "../include/stringTrim.h"

carFederate::carFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

carFederate::~carFederate() {
    finalize();
}

void carFederate::runFederate(const std::wstring& setFederateName, std::string setScenarioFilePath) {
    fedAmb = std::make_unique<carFedAmb>(rtiAmbassador.get());
    federateName = setFederateName;
    scenarioFilePath = setScenarioFilePath;

    connectToRTI();
    initializeFederation();
    joinFederation();
    achieveSyncPoint();
    initializeHandles();
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

void carFederate::joinFederation() {
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
    std::wcout << L"carFederate waiting for synchronization announcement..." << std::endl;

    while (fedAmb->syncLabel != L"InitialSync") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
}

void carFederate::initializeHandles() {
    try {
        fedAmb->carObjectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Car");

        // Get attribute handles
        fedAmb->carNameAttribute = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Name");
        fedAmb->licensePlateAttribute = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"LicensePlateNumber");
        fedAmb->fuelLevelAttribute = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"FuelLevel");
        fedAmb->fuelTypeAttribute = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"FuelType");
        fedAmb->positionAttribute = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Position");

        // Publish attributes
        rtiAmbassador->publishObjectClassAttributes(fedAmb->carObjectClassHandle, {
            fedAmb->positionAttribute,
            fedAmb->fuelTypeAttribute,
            fedAmb->fuelLevelAttribute,
            fedAmb->licensePlateAttribute,
            fedAmb->carNameAttribute
        });

        // Now register the object AFTER publishing
        fedAmb->carObjectInstanceHandle = rtiAmbassador->registerObjectInstance(fedAmb->carObjectClassHandle);

        // Retrieve interaction class handles before publishing them
        fedAmb->loadScenarioHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.LoadScenario");
        fedAmb->startHandle = rtiAmbassador->getInteractionClassHandle(L"Start");
        fedAmb->scenarioLoadedHandle = rtiAmbassador->getInteractionClassHandle(L"ScenarioLoaded");
        fedAmb->scenarioLoadFailureHandle = rtiAmbassador->getInteractionClassHandle(L"ScenarioLoadFailure");
        fedAmb->stopHandle = rtiAmbassador->getInteractionClassHandle(L"Stop");

        // Get parameter handles
        fedAmb->scenarioNameParam = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"ScenarioName");
        fedAmb->initialFuelParam = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"InitialFuelAmount");
        fedAmb->federateNameParam = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadedHandle, L"FederateName");
        fedAmb->loadErrorMessageParam = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadFailureHandle, L"FederateName");
        fedAmb->loadErrorMessageParam = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadFailureHandle, L"ErrorMessage");
        fedAmb->timeScaleFactorParam = rtiAmbassador->getParameterHandle(fedAmb->startHandle, L"TimeScaleFactor");

        // Publish interactions
        rtiAmbassador->publishInteractionClass(fedAmb->scenarioLoadedHandle);
        rtiAmbassador->publishInteractionClass(fedAmb->scenarioLoadFailureHandle);
        rtiAmbassador->publishInteractionClass(fedAmb->stopHandle);

        // Subscribe to interactions
        rtiAmbassador->subscribeInteractionClass(fedAmb->loadScenarioHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->startHandle);
        

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error initializing handles: " << e.what() << std::endl;
    }
}

void carFederate::run() {
    while (fedAmb->syncLabel != L"ShutdownSync") {
        if (fedAmb->syncLabel != L"ScenarioLoaded") {
            loadScenario();
            loadCarConfig(scenarioFilePath);
        }
        else if (fedAmb->syncLabel == L"ScenarioLoaded") {
            runSimulation();
        }
    }
}

void carFederate::loadScenario() {
    std::wcout << L"carFederate waiting for scenarioFederate synchronization..." << std::endl;

    while (fedAmb->syncLabel != L"ScenarioLoaded") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    if (fedAmb->scenarioFilePath.empty()) {
        std::cerr << "Error: Scenario file path is empty!" << std::endl;
        try {
            rti1516e::ParameterHandleValueMap parameters;
            parameters[fedAmb->federateNameParam] = rti1516e::HLAunicodeString(federateName).encode();
            parameters[fedAmb->loadErrorMessageParam] = rti1516e::HLAunicodeString(L"ERROR in carFederate").encode(); // Optional: Could include empty federateName or a custom error message

            // Publish the interaction for load failure
            rtiAmbassador->sendInteraction(fedAmb->scenarioLoadFailureHandle, parameters, rti1516e::VariableLengthData());
            std::wcout << "Published scenario load failure!" << std::endl;
        } catch (const rti1516e::Exception& e) {
            std::wcout << "Error publishing ScenarioLoadFailure: " << e.what() << std::endl;
        }
        return;
    }

    try {
        rti1516e::ParameterHandleValueMap parameters;
        parameters[fedAmb->federateNameParam] = rti1516e::HLAunicodeString(federateName).encode();  // Sending federate name

        // Publish the interaction for successful load
        rtiAmbassador->sendInteraction(fedAmb->scenarioLoadedHandle, parameters, rti1516e::VariableLengthData());
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing ScenarioLoaded: " << e.what() << std::endl;
    }

    std::ifstream configFile(fedAmb->scenarioFilePath);
    if (!configFile.is_open()) {
        std::cout << "Failed to open scenario config file: " << fedAmb->scenarioFilePath << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(configFile, line)) {
        size_t delimiterPos = line.find("=");
        if (delimiterPos == std::string::npos) continue;

        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        if (key == "TopLeftLat") {
            topLeftLat = std::stod(value);
        } else if (key == "TopLeftLong") {
            topLeftLong = std::stod(value);
        } else if (key == "BottomRightLat") {
            bottomRightLat = std::stod(value);
        } else if (key == "BottomRightLong") {
            bottomRightLong = std::stod(value);
        } else if (key == "StartLat") {
            startLat = std::stod(value);
        } else if (key == "StartLong") {
            startLong = std::stod(value);
        } else if (key == "StopLat") {
            goalLat = std::stod(value);
        } else if (key == "StopLong") {
            goalLong = std::stod(value);
        }
    }

    fuelLevel = fedAmb->initialFuel;
    configFile.close();
}

void carFederate::loadCarConfig(std::string filePath) {
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open car config file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        size_t delimiterPos = line.find("=");
        if (delimiterPos == std::string::npos) continue;

        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        if (key == "Name") {
            carName = value;
        } else if (key == "LicensePlate") {
            licensePlate = value;
        } else if (key == "FuelType") {
            fuelType = value;
        } else if (key == "NormalSpeed") {
            normalSpeed = std::stod(value);
        } else if (key == "LitersPer100km1") {
            fuelConsumption1 = std::stod(value);
        } else if (key == "LitersPer100km2") {
            fuelConsumption2 = std::stod(value);
        } else if (key == "LitersPer100km3") {
            fuelConsumption3 = std::stod(value);
        }
    }
    printf("[DEBUG] Car config loaded: %s\n", carName.c_str());
    configFile.close();
}

void carFederate::updateAttributes() {
    try {
        rti1516e::AttributeHandleValueMap attributes;
        rti1516e::HLAfixedRecord position;
        position.appendElement(rti1516e::HLAfloat64BE(startLat));
        position.appendElement(rti1516e::HLAfloat64BE(startLong));

        attributes[fedAmb->positionAttribute] = position.encode();

        // Convert std::string to std::wstring
        std::wstring wCarName(carName.begin(), carName.end());
        std::wstring wLicensePlate(licensePlate.begin(), licensePlate.end());
        std::wstring wFuelType(fuelType.begin(), fuelType.end());

        attributes[fedAmb->carNameAttribute] = rti1516e::HLAunicodeString(wCarName).encode();
        attributes[fedAmb->licensePlateAttribute] = rti1516e::HLAunicodeString(wLicensePlate).encode();
        attributes[fedAmb->fuelTypeAttribute] = rti1516e::HLAunicodeString(wFuelType).encode();
        attributes[fedAmb->fuelLevelAttribute] = rti1516e::HLAfloat32BE(fuelLevel).encode();

        rtiAmbassador->updateAttributeValues(fedAmb->carObjectInstanceHandle, attributes, rti1516e::VariableLengthData());


        std::cout << "[DEBUG] Published updated car attributes: " << carName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating and publishing attributes: " << e.what() << std::endl;
    }
}

/*
TODO: Implement a better solution for the simulation loop.
    Car run too fast
    Insert random speed changes
    Fuel consumption in smaller steps
    Fix issues with cout not working as intended
    Create a Federate that publish when car has reached goal?
*/
void carFederate::runSimulation() {
    auto startTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = 0.0;
    double count = 0;

    double speedInKmPerSecond = normalSpeed / 3600.0;
    double fuelConsumption = (fuelConsumption3 / 100) * speedInKmPerSecond;
    double totalDistance = haversineDistance(startLat, startLong, goalLat, goalLong);
    double distanceTraveled = 0.0;
    std::wcout << L"[DEBUG] runSimulation started!" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Only for cancel purpose
    while(totalDistance >= distanceTraveled && fuelLevel > 0) {
        try {
            auto currentTime = std::chrono::high_resolution_clock::now();
            elapsedTime = std::chrono::duration<double>(currentTime - startTime).count();
            elapsedTime = (elapsedTime * fedAmb->timeScaleFactor) - fedAmb->timeScaleFactor;

            std::wcout << L"Elapsed time: " << elapsedTime << std::endl;
            double traveledRatio = distanceTraveled / totalDistance;
            double currentLat = startLat + (goalLat - startLat) * traveledRatio;
            double currentLong = startLong + (goalLong - startLong) * traveledRatio;
            updateCarPosition(currentLat, currentLong);
            
            distanceTraveled = speedInKmPerSecond * elapsedTime;
            
            if (totalDistance <= distanceTraveled) {
                std::wcout << L"Car has reached the goal!" << std::endl;
            }

            // Need better solution then this.
            if(count < elapsedTime) {
                fuelLevel -= fuelConsumption;
                std::wcout << "[DEBUG] Fuel level: " << fuelLevel << std::endl;
                std::wcout << "[DEBUG] Distance traveled: " << distanceTraveled << ", totalDistance: " << totalDistance << std::endl;
                count++;
            }
            
            if (fuelLevel < 0) {
                std::wcout << "Car has run out of fuel!" << std::endl;
            }

            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        } catch (const rti1516e::Exception& e) {
            std::wcout << "Error updating car state: " << e.what() << std::endl;
        }
    }
    std::cout << carName << " has finnished the simulation." << std::endl;
}

void carFederate::updateCarPosition(double newLat, double newLong) {
    try {
        rti1516e::AttributeHandleValueMap attributes;

        rti1516e::HLAfixedRecord position;
        position.appendElement(rti1516e::HLAfloat64BE(newLat));
        position.appendElement(rti1516e::HLAfloat64BE(newLong));

        attributes[fedAmb->positionAttribute] = position.encode();

        rtiAmbassador->updateAttributeValues(fedAmb->carObjectInstanceHandle, attributes, rti1516e::VariableLengthData());

        std::wcout << "Updated position: Latitude = " << newLat << ", Longitude = " << newLong << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating position attribute: " << e.what() << std::endl;
    }
}

void carFederate::finalize() {
    try {
        // Unpublish interaction classes
        rtiAmbassador->unpublishInteractionClass(fedAmb->scenarioLoadedHandle);
        rtiAmbassador->unpublishInteractionClass(fedAmb->scenarioLoadFailureHandle);
        rtiAmbassador->unpublishInteractionClass(fedAmb->stopHandle);
    
        // Unsubscribe from interaction classes
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->loadScenarioHandle);
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->startHandle);
    
        std::wcout << "Successfully unpublished and unsubscribed from interactions." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << "Error during unpublishing or unsubscribing: " << e.what() << std::endl;
    }
    resignFederation();
}

void carFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}