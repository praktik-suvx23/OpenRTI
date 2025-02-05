#include "../include/masterFederate.h"
#include "../include/masterFedAmbassador.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream> // For openFileCheck

// ================================
// This function is temporary / for debugging purposes
void openFileCheck() {
    std::cout << "Debugg-Message 1" << std::endl;

    std::ifstream file("foms/FOM.xml");

    /*
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    */

    if (file.is_open()) {
        std::cout << "FOM.xml file successfully opened!" << std::endl;
    } else {
        std::cout << "Error: Unable to open FOM.xml file at path: foms/FOM.xml" << std::endl;
    }

    file.close();

    std::cout << "End Debugg-Message 1" << std::endl;
}
// ================================

masterFederate::masterFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

masterFederate::~masterFederate() {
    finalize();
}

void masterFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<masterFedAmbassador>(*this);

    std::cout << "Calling connectToRTI" << std::endl;
    connectToRTI();

    std::cout << "Calling initializeFederation" << std::endl;
    initializeFederation();

    std::cout << "Calling joinFederation" << std::endl;
    joinFederation();

    std::cout << "Calling registerSyncPoint" << std::endl;
    registerSyncPoint();

    std::cout << "Calling achiveSyncPoint" << std::endl;
    achiveSyncPoint();

    std::cout << "Calling initializeHandles" << std::endl;
    initializeHandles(0.0, 0.0);

    std::cout << "Calling finalize" << std::endl;
    finalize();

    std::cout << "Calling resignFederation" << std::endl;
    resignFederation();
}

void masterFederate::connectToRTI(){
    try{
        if (!rtiAmbassador) {
            std::wcout << L"RTI Ambassador is null, cannot connect!" << std::endl;
            return;
        }

        std::cout << "Connecting to RTI..." << std::endl;
        //rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED);
        rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
        std::wcout << L"Connected to the RTI" << std::endl;

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

void masterFederate::initializeFederation() {
    try{
        std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
        rtiAmbassador->createFederationExecution(L"ExampleFederation", fomModules);
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch(rti1516e::Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }
}

void masterFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation");
        std::wcout << "Joined federation as ExampleFederate." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error joining federation: " << e.what() << std::endl;
    }
}

void masterFederate::registerSyncPoint() {
    try {
        std::wstring syncPointLabel = L"SYNC_START";  // Name of the sync point
        rti1516e::VariableLengthData userSuppliedTag; // Optional user data (empty here)

        // Register the synchronization point with RTI
        rtiAmbassador->registerFederationSynchronizationPoint(syncPointLabel, userSuppliedTag);
        std::wcout << "Registered Sync Point: " << syncPointLabel << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Failed to register sync point: " << e.what() << std::endl;
    }
}

void masterFederate::achiveSyncPoint() {
    try {
        std::wstring syncPointLabel = L"SYNC_START";  // Name of the sync point

        // Wait for the RTI to notify that the sync point has been achieved
        while (!fedAmb->isReadyToRun) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << "Achieved sync point: " << syncPointLabel << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Failed to achieve sync point: " << e.what() << std::endl;
    }
}

void masterFederate::initializeHandles(double position, double speed) {
    try {
        vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");        
        positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        attributeHandleSet.insert(positionHandle);
        attributeHandleSet.insert(speedHandle);

        std::wcout << "VehicleClassHandle: " << vehicleClassHandle <<
        "\nPositionHandle: " << positionHandle <<
        "\nSpeedHandle: " << speedHandle <<
        ".\nInitiated \"Position\" & \"Speed\" for \"Vehicle\"." << std::endl;

        attributeHandleSet.insert(positionHandle);
        attributeHandleSet.insert(speedHandle);
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error initializing handles: " << e.what() << std::endl;
    }
}

void masterFederate::finalize() {
    try {
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void masterFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}