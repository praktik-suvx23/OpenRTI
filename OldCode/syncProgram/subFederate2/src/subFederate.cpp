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

#include "../include/subFederate.h"
#include "../include/subFedAmb.h"

subFederate::subFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

subFederate::~subFederate() {
    finalize();
    std::cout << "Last value subscribed on: " << fedAmb->savedData << std::endl;
}

void subFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<subFedAmb>(rtiAmbassador.get());

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

    std::cout << "Finalizing federate..." << std::endl;
    finalize();
}

void subFederate::connectToRTI() {
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

void subFederate::initializeFederation() {
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

void subFederate::joinFederation(std::wstring federateName) {
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

void subFederate::achiveSyncPoint() {
    std::wcout << L"Publisher Federate waiting for synchronization announcement..." << std::endl;

    // Process callbacks until the sync point is announced
    while (fedAmb->syncLabel != L"InitialSync") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    std::wcout << L"Publisher Federate received sync point. Ready to achieve it." << std::endl;
}

void subFederate::initializeHandles() {
    try {
        fedAmb->interactionClassHandle1 = rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InteractionClass1");
        fedAmb->parameterHandle1 = rtiAmbassador->getParameterHandle(fedAmb->interactionClassHandle1, L"Parameter1");
        rtiAmbassador->subscribeInteractionClass(fedAmb->interactionClassHandle1);
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

void subFederate::run() {
    while (true) {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);

        if(fedAmb->syncLabel == L"ShutdownSync") {
            break;
        }
    }
}

void subFederate::finalize() {
    try {
        rtiAmbassador->unsubscribeInteractionClass(fedAmb->interactionClassHandle1);
        std::wcout << L"Unpublished InteractionClass1" << std::endl;
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void subFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}