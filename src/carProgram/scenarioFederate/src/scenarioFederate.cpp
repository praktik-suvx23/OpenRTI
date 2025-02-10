#include "../include/scenarioFederate.h"
#include "../include/scenarioFedAmb.h"

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include <unordered_map>

scenarioFederate::scenarioFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

scenarioFederate::~scenarioFederate() {
    finalize();
}

void scenarioFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<scenarioFedAmb>(rtiAmbassador.get());

    std::cout << "Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
    connectToRTI();

    std::cout << "Creating federation..." << std::endl;
    initializeFederation();

    std::cout << "Joining federation..." << std::endl;
    joinFederation(federateName);

    std::cout << "Achieving sync point..." << std::endl;
    achiveSyncPoint();

    std::cout << "Initializing handles..." << std::endl;
    initializeHandles();

    std::cout << "Running federate..." << std::endl;
    run();
}

void scenarioFederate::connectToRTI() {
    try{
        if(!rtiAmbassador){
            std::cout << "RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (rti1516e::ConnectionFailed& e){
        std::wcout << L"Connection failed: " << e.what() << std::endl;
    } catch (rti1516e::InvalidLocalSettingsDesignator& e){
        std::wcout << L"Invalid local settings designator: " << e.what() << std::endl;
    } catch (rti1516e::UnsupportedCallbackModel& e){
        std::wcout << L"Unsupported callback model: " << e.what() << std::endl;
    } catch (rti1516e::AlreadyConnected& e){
        std::wcout << L"Already connected: " << e.what() << std::endl;
    } catch (rti1516e::CallNotAllowedFromWithinCallback& e){
        std::wcout << L"Call not allowed from within callback: " << e.what() << std::endl;
    }
    catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception in runFederate: " << e.what() << std::endl;
    } 
    catch (const std::exception& e) {
        std::wcout << L"Standard Exception in runFederate: " << e.what() << std::endl;
    } 
    catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void scenarioFederate::initializeFederation() {
    federationName = L"exampleFederation";
    fomModule = L"foms/FOM.xml";
    mimModule = L"foms/MIM.xml";
    fomModules = {fomModule};
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const rti1516e::CouldNotOpenFDD&) {
        std::wcout << "Could not open FDD: " << fomModule << std::endl;
    } catch (const rti1516e::ErrorReadingFDD&) {
        std::wcout << "Error reading FDD: " << fomModule << std::endl;
    } catch (const rti1516e::CouldNotOpenMIM&) {
        std::wcout << "Could not open MIM: " << mimModule << std::endl;
    } catch (const rti1516e::ErrorReadingMIM&) {
        std::wcout << "Error reading MIM: " << mimModule << std::endl;
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

void scenarioFederate::joinFederation(std::wstring federateName) {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
    } catch (const rti1516e::FederateAlreadyExecutionMember&) {
        std::wcout << L"Federate already execution member: " << federateName << std::endl;
    } catch (const rti1516e::FederationExecutionDoesNotExist&) {
        std::wcout << L"Federation execution does not exist: " << federationName << std::endl;
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

void scenarioFederate::achiveSyncPoint() {    
    std::wcout << L"Publisher Federate waiting for synchronization announcement..." << std::endl;

    // Process callbacks until the sync point is announced
    while (fedAmb->syncLabel != L"InitialSync") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    std::wcout << L"Publisher Federate received sync point. Ready to achieve it." << std::endl;
}

void scenarioFederate::initializeHandles() {
    try {
        // Initialize interaction class handles for interactions related to the scenario
        fedAmb->loadScenarioHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.LoadScenario");
        fedAmb->scenarioLoadedHandle = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.ScenarioLoaded");
        
        // Initialize parameter handles for the interactions
        fedAmb->scenarioNameParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"ScenarioName");
        fedAmb->topLeftLatParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"TopLeftLat");
        fedAmb->topLeftLongParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"TopLeftLong");
        fedAmb->bottomRightLatParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"BottomRightLat");
        fedAmb->bottomRightLongParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"BottomRightLong");
        fedAmb->startLatParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StartLat");
        fedAmb->startLongParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StartLong");
        fedAmb->stopLatParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StopLat");
        fedAmb->stopLongParamHandle = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"StopLong");
        rtiAmbassador->publishInteractionClass(fedAmb->loadScenarioHandle);
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception while initializing handles: " << e.what() << std::endl;
    }
}


void scenarioFederate::run() {
    std::cout << "Choose Scenario:\n1. Load Scenario A\n2. Load Scenario B" << std::endl;
    std::string temp = "";
    while(temp != "1" && temp != "2") {
        std::cin >> temp;
    }

    std::string scenarioFile;
    std::wstring scenarioName;

    if (temp == "1") {
        scenarioFile = "scenarios/scenarioA.txt";
        scenarioName = L"ScenarioA";
    } else {
        scenarioFile = "scenarios/scenarioB.txt";
        scenarioName = L"ScenarioB";
    }

    loadScenario(scenarioFile);

    publishScenario(scenarioName);
}

void scenarioFederate::loadScenario(std::string filePath){
    std::ifstream configFile(filePath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file." << std::endl;
        return;
    }

    // Load the scenario configuration
    std::string line;
    while (std::getline(configFile, line)) {
        // Parse key-value pairs from each line (assuming each line follows "Key=Value")
        if (line.find("TopLeftLat=") != std::string::npos) {
            fedAmb->scenarioTopLeftLat = std::stod(line.substr(11));  // Assuming it's of the format "TopLeftLat=47.715699"
        } else if (line.find("TopLeftLong=") != std::string::npos) {
            fedAmb->scenarioTopLeftLong = std::stod(line.substr(12));
        } else if (line.find("BottomRightLat=") != std::string::npos) {
            fedAmb->scenarioBotRightLat = std::stod(line.substr(15));
        } else if (line.find("BottomRightLong=") != std::string::npos) {
            fedAmb->scenarioBotRightLong = std::stod(line.substr(16));
        } else if (line.find("StartLat=") != std::string::npos) {
            fedAmb->scenarioStartLat = std::stod(line.substr(9));
        } else if (line.find("StartLong=") != std::string::npos) {
            fedAmb->scenarioStartLong = std::stod(line.substr(10));
        } else if (line.find("StopLat=") != std::string::npos) {
            fedAmb->scenarioEndLat = std::stod(line.substr(8));
        } else if (line.find("StopLong=") != std::string::npos) {
            fedAmb->scenarioEndLong = std::stod(line.substr(9));
        }
    }
    configFile.close();
}

void scenarioFederate::publishScenario(std::wstring scenarioName) {
    try {
        rti1516e::ParameterHandleValueMap parameters;

        // Convert values to HLA format
        rti1516e::HLAunicodeString hlaScenarioName(scenarioName);
        rti1516e::HLAfloat64BE hlaTopLeftLat(fedAmb->scenarioTopLeftLat);
        rti1516e::HLAfloat64BE hlaTopLeftLong(fedAmb->scenarioTopLeftLong);
        rti1516e::HLAfloat64BE hlaBottomRightLat(fedAmb->scenarioBotRightLat);
        rti1516e::HLAfloat64BE hlaBottomRightLong(fedAmb->scenarioBotRightLong);
        rti1516e::HLAfloat64BE hlaStartLat(fedAmb->scenarioStartLat);
        rti1516e::HLAfloat64BE hlaStartLong(fedAmb->scenarioStartLong);
        rti1516e::HLAfloat64BE hlaStopLat(fedAmb->scenarioEndLat);
        rti1516e::HLAfloat64BE hlaStopLong(fedAmb->scenarioEndLong);

        // Assign encoded values to the parameter handle map
        parameters[fedAmb->scenarioNameParamHandle] = hlaScenarioName.encode();
        parameters[fedAmb->topLeftLatParamHandle] = hlaTopLeftLat.encode();
        parameters[fedAmb->topLeftLongParamHandle] = hlaTopLeftLong.encode();
        parameters[fedAmb->bottomRightLatParamHandle] = hlaBottomRightLat.encode();
        parameters[fedAmb->bottomRightLongParamHandle] = hlaBottomRightLong.encode();
        parameters[fedAmb->startLatParamHandle] = hlaStartLat.encode();
        parameters[fedAmb->startLongParamHandle] = hlaStartLong.encode();
        parameters[fedAmb->stopLatParamHandle] = hlaStopLat.encode();
        parameters[fedAmb->stopLongParamHandle] = hlaStopLong.encode();

        // Send the interaction
        rtiAmbassador->sendInteraction(fedAmb->loadScenarioHandle, parameters, rti1516e::VariableLengthData());
        std::cout << "Published LoadScenario interaction." << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error sending LoadScenario interaction: " << e.what() << std::endl;
    }
}

void scenarioFederate::finalize() {
    try {
        rtiAmbassador->unpublishInteractionClass(fedAmb->loadScenarioHandle);
        std::wcout << L"Unpublished InteractionClass1" << std::endl;
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void scenarioFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}