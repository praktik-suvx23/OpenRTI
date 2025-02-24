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
    fomModule = L"" FOM_MODULE_PATH;
    mimModule = L"" MIM_MODULE_PATH;
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
        // Get interaction class handles
        fedAmb->loadScenarioHandle = rtiAmbassador->getInteractionClassHandle(L"LoadScenario");
        fedAmb->scenarioLoadedHandle = rtiAmbassador->getInteractionClassHandle(L"ScenarioLoaded");
        fedAmb->scenarioLoadFailureHandle = rtiAmbassador->getInteractionClassHandle(L"ScenarioLoadFailure");
        fedAmb->startHandle = rtiAmbassador->getInteractionClassHandle(L"Start");
        fedAmb->stopHandle = rtiAmbassador->getInteractionClassHandle(L"Stop");

        // Get parameter handles using fedAmb-> interaction handles
        fedAmb->scenarioNameParam = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"ScenarioName");
        fedAmb->initialFuelParam = rtiAmbassador->getParameterHandle(fedAmb->loadScenarioHandle, L"InitialFuelAmount");
        fedAmb->federateNameParam_Loaded = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadedHandle, L"FederateName");
        fedAmb->federateNameParam_Failed = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadFailureHandle, L"FederateName");
        fedAmb->errorMessageParam = rtiAmbassador->getParameterHandle(fedAmb->scenarioLoadFailureHandle, L"ErrorMessage");
        fedAmb->timeScaleFactorParam = rtiAmbassador->getParameterHandle(fedAmb->startHandle, L"TimeScaleFactor");

        std::wcout << L"Interaction class handles initialized successfully." << std::endl;
    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"Error initializing interaction handles: " << e.what() << std::endl;
    }

    try{
        rtiAmbassador->publishInteractionClass(fedAmb->loadScenarioHandle);
        rtiAmbassador->publishInteractionClass(fedAmb->startHandle);
        //rtiAmbassador->publishInteractionClass(fedAmb->stopHandle); 

        rtiAmbassador->subscribeInteractionClass(fedAmb->scenarioLoadedHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->scenarioLoadFailureHandle);
        rtiAmbassador->subscribeInteractionClass(fedAmb->stopHandle);
    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"Error subscribing to interaction classes: " << e.what() << std::endl;
    }
}

void scenarioFederate::run() {
    

    loadScenario();
    registerSyncPoint();
    checkAndStartSimulation();
}

void scenarioFederate::loadScenario() {
    std::cout << "Choose Scenario:\n1. Load Scenario A\n2. Load Scenario B" << std::endl;
    std::string temp = "";
    while(temp != "1" && temp != "2") {
        std::cin >> temp;
    }

    std::string scenarioFile;
    std::wstring scenarioName;

    if (temp == "1") {
        scenarioFile = SCENARIO_MODULE_PATH_A;
        scenarioName = L"" SCENARIO_MODULE_PATH_A;
    } else {
        scenarioFile = SCENARIO_MODULE_PATH_B;
        scenarioName = L"" SCENARIO_MODULE_PATH_B;
    }

    std::ifstream file(scenarioFile);
    if (!file.is_open()) {
        std::cout << "Failed to open config file." << std::endl;
        return;
    }

    double initialFuelAmount = 0.0;
    std::cout << "Enter initial fuel amount: " << std::endl;
    std::cin >> initialFuelAmount;

    double timeScaleFactor = 1.0;
    std::cout << "Enter time scale factor: " << std::endl;
    std::cin >> timeScaleFactor;
    
    rti1516e::ParameterHandleValueMap parameters;
    
    parameters[fedAmb->scenarioNameParam] = rti1516e::HLAunicodeString(scenarioName).encode();
    parameters[fedAmb->initialFuelParam] = rti1516e::HLAinteger32BE(initialFuelAmount).encode();

    rtiAmbassador->sendInteraction(fedAmb->loadScenarioHandle, parameters, rti1516e::VariableLengthData());

    rti1516e::ParameterHandleValueMap startParameters;

    startParameters[fedAmb->timeScaleFactorParam] = rti1516e::HLAfloat32BE(timeScaleFactor).encode();

    rtiAmbassador->sendInteraction(fedAmb->startHandle, startParameters, rti1516e::VariableLengthData());



    std::wcout << L"Sent LoadScenario interaction." << std::endl;
}

void scenarioFederate::registerSyncPoint(){
    try{
        rtiAmbassador->registerFederationSynchronizationPoint(L"ScenarioLoaded", rti1516e::VariableLengthData());
        while(fedAmb->syncLabel != L"ScenarioLoaded") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"scenarioFederate has announced synchronization point: ScenarioLoaded" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while registering synchronization point: " << e.what() << std::endl;
    }
}


/* ========================================
    Don't know what to do with this method.
    Mostly wanted to try and use all the Interactions 
    / parameters aviable in the FOM.
    =============================================*/
void scenarioFederate::checkAndStartSimulation() {
    int counter = 0;

    while(!fedAmb->federateConnectedSuccessfully && counter < 10) {
        std::cout << "Waiting for federates to load scenario... (" << counter + 1 << ")" << std::endl;
        try{
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        } catch (const rti1516e::Exception& e) {
            std::wcout << L"Error during simulation start: " << e.what() << std::endl;
            return;
        }
        counter++;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    if (counter == 10) {
        std::wcout << L"Timeout waiting for federates to load scenario. Exiting..." << std::endl;
        return;
    }

    if (fedAmb->federateConnectedSuccessfully) {

        std::wcout << L"Simulation started." << std::endl;
        fedAmb->syncLabel = L"";
        fedAmb->federateConnectedSuccessfully = false;
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