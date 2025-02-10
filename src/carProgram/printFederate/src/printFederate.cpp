#include "../include/printFederate.h"
#include "../include/printFedAmb.h"

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <unordered_map>

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
    std::cout << "Press 'Enter' to register synchronization point..." << std::endl;
    userInteraction();

    try {
        // Register the synchronization point with the RTI
        rtiAmbassador->registerFederationSynchronizationPoint(L"InitialSync", rti1516e::VariableLengthData());
        std::wcout << L"Master Federate waiting for synchronization..." << std::endl;
    
        // Announce the synchronization point to inform other federates
        while (fedAmb->syncLabel != L"InitialSync") { 
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"Master Federate has announced synchronization point: InitialSync" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while registering synchronization point: " << e.what() << std::endl;
    }
}

void mastFederate::achiveSyncPoint() {    
    std::wcout << L"Publisher Federate waiting for synchronization announcement..." << std::endl;

    // Process callbacks until the sync point is announced
    while (fedAmb->syncLabel != L"InitialSync") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    std::wcout << L"Publisher Federate received sync point. Ready to achieve it." << std::endl;
}

void mastFederate::initializeHandles() {
    try {
        // Initialize the handles for car attributes
        fedAmb->carObjectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Car");

        // Get the attribute handles for Name, LicensePlateNumber, and Position
        fedAmb->nameAttributeHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Name");
        fedAmb->licensePlateAttributeHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"LicensePlateNumber");
        fedAmb->positionAttributeHandle = rtiAmbassador->getAttributeHandle(fedAmb->carObjectClassHandle, L"Position");
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception while initializing handles: " << e.what() << std::endl;
    }
    
    try {
        // Subscribe to the object class (Car) for the attributes we need
        rtiAmbassador->subscribeObjectClassAttributes(fedAmb->carObjectClassHandle, { fedAmb->nameAttributeHandle, fedAmb->licensePlateAttributeHandle, fedAmb->positionAttributeHandle });
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception while subscribing to object class: " << e.what() << std::endl;
    }
}


void mastFederate::run() {

}

void mastFederate::finalize() {
    try {
        rtiAmbassador->unpublishInteractionClass(interactionClassHandle);
        std::wcout << L"Unpublished InteractionClass1" << std::endl;
        resignFederation();
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