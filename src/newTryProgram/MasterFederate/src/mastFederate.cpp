#include "../include/mastFederate.h"
#include "../include/mastFedAmb.h"

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
//#include "include/MyPositionDecoder.h"
//#include "include/MyFloat32Decoder.h"
//#include "include/ObjectInstanceHandleHash.h"
#include <iostream>
#include <thread>
#include <chrono>

#include <unordered_map>

// temporary
#include <fstream>
#include <locale>
#include <codecvt>

// ================================
// This function is temporary / for debugging purposes
void openFileCheck(std::string checkFile) {
    std::cout << "Debugg-Message 1" << std::endl;

    std::ifstream file(checkFile);

    //std::string line;
    //while (std::getline(file, line)) {
    //    std::cout << line << std::endl;
    //}

    if (!file.is_open()) {
        std::cout << L"ERROR: Could not open file: " << checkFile << std::endl;
    } else {
        std::cout << L"SUCCESS: Opened file: " << checkFile << std::endl;
    }

    std::cout << "End Debugg-Message 1" << std::endl;
}

mastFederate::mastFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

mastFederate::~mastFederate() {
    finalize();
}

void mastFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<mastFedAmb>(rtiAmbassador.get());

    std::cout << "Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
    connectToRTI();

    std::cout << "Creating federation..." << std::endl;
    initializeFederation();

    std::cout << "Joining federation..." << std::endl;
    joinFederation(federateName);

    std::cout << "Registering sync point..." << std::endl;
    registerSyncPoint();

    std::cout << "Achieving sync point..." << std::endl;
    achiveSyncPoint();

    std::cout << "Initializing handles..." << std::endl;
    initializeHandles();

    std::cout << "Running federate..." << std::endl;
    run();
}

void mastFederate::connectToRTI() {
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

void mastFederate::initializeFederation() {
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

void mastFederate::joinFederation(std::wstring federateName) {
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

void mastFederate::registerSyncPoint() {

}

void mastFederate::achiveSyncPoint() {

}

void mastFederate::initializeHandles() {
    try{
        fedAmb->interactionClassHandle1 = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        fedAmb->parameterHandle1 = rtiAmbassador->getParameterHandle(fedAmb->interactionClassHandle1, L"Parameter1");
    } catch (const rti1516e::NameNotFound& e){
        std::wcout << L"Name not found: " << e.what() << std::endl;
    } catch (const rti1516e::InvalidInteractionClassHandle& e){
        std::wcout << L"Invalid interaction class handle: " << e.what() << std::endl;
    } catch (const rti1516e::InvalidObjectClassHandle& e){
        std::wcout << L"Invalid object class handle: " << e.what() << std::endl;
    } catch (const rti1516e::InvalidAttributeHandle& e){
        std::wcout << L"Invalid attribute handle: " << e.what() << std::endl;
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

void mastFederate::run() {
    std::cout << "Program running until user presses \"Enter\"" << std::endl;
    std::string input = "temp";
    while(true){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        std::getline(std::cin, input);
        if (input == "") {
            break;
        }
    }
}

void mastFederate::finalize() {
    try {
        resignFederation();
        destroyFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void mastFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}

void mastFederate::destroyFederation() {
    try {
        std::wcout << "Attempting to destroy federation..." << std::endl;
        rtiAmbassador->destroyFederationExecution(federationName);
        std::wcout << "Federation destroyed successfully." << std::endl;
    } catch (const rti1516e::FederatesCurrentlyJoined&) {
        std::wcout << "Error: Cannot destroy federation; other federates are still joined." << std::endl;
    } catch (const rti1516e::FederationExecutionDoesNotExist&) {
        std::wcout << "Error: Federation does not exist or was already destroyed." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error destroying federation: " << e.what() << std::endl;
    }
}