#include "../include/carFederate.h"
#include "../include/carFedAmbassador.h"
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

carFederate::carFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

carFederate::~carFederate() {
    finalize();
}

void carFederate::runFederate(const std::wstring& federateName) {
    fedAmb = std::make_unique<carFederateAmbassador>(*this);

        


    std::cout << "Calling connectToRTI" << std::endl;
    connectToRTI();

    std::cout << "Calling initializeFederation" << std::endl;
    initializeFederation();

    std::cout << "Calling joinFederation" << std::endl;
    joinFederation();

    std::cout << "Calling achiveSyncPoint" << std::endl;
    achiveSyncPoint();

    std::cout << "Calling initializeHandles" << std::endl;
    initializeHandles(9.9, 9.9);

    std::cout << "Calling subscribeOnly" << std::endl;
    subscribeOnly();

    std::cout << "Calling publishOnly" << std::endl;
    publishOnly();

    rtiAmbassador->enableAttributeScopeAdvisorySwitch();
    vehicleInstanceHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);
    std::wcout << "Vehicle instance handle: " << vehicleInstanceHandle << std::endl;
    std::cout << "Calling run" << std::endl;
    run();

    std::cout << "Calling printCurrentSubscribedValues" << std::endl;
    printCurrentSubscribedValues();

    std::cout << "Calling finalize" << std::endl;
    finalize();

    std::cout << "Calling resignFederation" << std::endl;
    resignFederation();
}

void carFederate::connectToRTI(){
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

void carFederate::initializeFederation() {
    try{
        std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
        rtiAmbassador->createFederationExecution(L"ExampleFederation", fomModules);
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch(rti1516e::Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }
}

void carFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation");
        std::wcout << "Joined federation as ExampleFederate." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error joining federation: " << e.what() << std::endl;
    }
}

void carFederate::achiveSyncPoint() {
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

void carFederate::initializeHandles(double position, double speed) {
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

void carFederate::publishOnly() {
    try {
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, attributeHandleSet);
        std::wcout << "Published attributes for Vehicle." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing attributes: " << e.what() << std::endl;
    }
}

void carFederate::subscribeOnly() {
    try {
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, attributeHandleSet);
        std::wcout << "Subscribed to attributes for Vehicle." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error subscribing to attributes: " << e.what() << std::endl;
    }
}

void carFederate::run() {
    std::cout << "Federate running." << std::endl;

    double position = 0.0;
    double speed = 0.0;

    try{
        for(int i = 0; i < 10; i++) {
            speed += 0.5 +(speed * 0.2);
            position += speed * 1.5;

            std::cout << "Iteration: " << i+1 << std::endl;
            updateAttributes(position, speed);

            rtiAmbassador->evokeMultipleCallbacks(0.1, 0.2);

            //SUBSCRIBE FUNCTION, subscribe on the updated values. Print them out.

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating attributes: " << e.what() << std::endl;
    }
}

void carFederate::updateAttributes(double position, double speed) {
    try {
        rti1516e::VariableLengthData positionData(&position, sizeof(position));
        rti1516e::VariableLengthData speedData(&speed, sizeof(speed));

        attributeValues[positionHandle] = positionData;
        attributeValues[speedHandle] = speedData;

        rtiAmbassador->updateAttributeValues(vehicleInstanceHandle, attributeValues, tag);
        std::wcout << "[DE-BUG] Updated Position: " << position << ", Speed: " << speed << std::endl;
        
        // DE-BUG: Reflect the updated values
        //auto logicalTime = rti1516e::HLAinteger64TimeFactory().makeInitial();
        //fedAmb->reflectAttributeValues(vehicleInstanceHandle, attributeValues, tag, rti1516e::RECEIVE, rti1516e::BEST_EFFORT, *logicalTime, rti1516e::RECEIVE, rti1516e::MessageRetractionHandle(), rti1516e::SupplementalReflectInfo());
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating attributes: " << e.what() << std::endl;
    }
}

void carFederate::printCurrentSubscribedValues() {
    std::cout << "[INFO] Current Subscribed Vehicle Values:" << std::endl;
    std::cout << "Position: " << fedAmb->lastReceivedPosition << std::endl;
    std::cout << "Speed: " << fedAmb->lastReceivedSpeed << std::endl;
}

void carFederate::finalize() {
    try {
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