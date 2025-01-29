#include "../include/subFederate.h"
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

Federate::Federate() : _state(0), _federateAmbassador(std::make_unique<FederateAmbassador>()) {
    _rtiAmbassador = std::make_unique<rti1516e::RTIambassadorFactory>()->createRTIambassador();
}

Federate::~Federate() {
    finalize();
}

void Federate::runFederate(const std::wstring& federateName) {
    this->fedAmb = new FederateAmbassador();
    //openFileCheck();

    std::cout << "Calling connectToRTI" << std::endl;
    connectToRTI();

    std::cout << "Calling initializeFederation" << std::endl;
    initializeFederation();


    std::cout << "Calling joinFederation" << std::endl;
    joinFederation();

    std::cout << "Calling subscribeToAttributes" << std::endl;
    subscribeToAttributes();

    std::cout << "Calling run" << std::endl;
    run();
}

void Federate::connectToRTI(){
    try{
        std::cout << "Message01: For some *Unknown reason* this message..." << std::endl;
        _rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED);
        std::wcout << L"Connected to the RTI" << std::endl;
        std::wcout << "Message02: ... and this message seems requered for the program to progress..?" << std::endl;

        if (!_rtiAmbassador) {
            std::wcout << L"RTI Ambassador is null after connection attempt!" << std::endl;
            return;
        }

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

void Federate::initializeFederation() {
    try{
        std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
        _rtiAmbassador->createFederationExecution(L"ExampleFederation", fomModules);
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch(rti1516e::Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }
}

void Federate::joinFederation() {
    try {
        _rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation");
        std::wcout << "Joined federation as ExampleFederate." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error joining federation: " << e.what() << std::endl;
    }
}

void Federate::subscribeToAttributes() {
    try {
        vehicleClassHandle = _rtiAmbassador->getObjectClassHandle(L"Vehicle");
        positionHandle = _rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        speedHandle = _rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(positionHandle);
        attributes.insert(speedHandle);

        _rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, attributes);
        std::wcout << "Subscribed to Vehicle attributes: Position and Speed." << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error subscribing to attributes: " << e.what() << std::endl;
    }
}

void Federate::run() {
    std::cout << "Federate running." << std::endl;

    bool receivedNewData = false;  // Flag to track if new data has been received

    // Set initial lastUpdateTime before starting the loop
    lastUpdateTime = std::chrono::steady_clock::now();

    while (!_done) {
        // Poll the RTI for messages (this is where you would normally check for updates)
        // _rtiAmbassador->tick(); // Uncomment and replace with correct method

        // Check if more than 15 seconds have passed since the last update
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime).count() > 15) {
            if (!receivedNewData) {
                std::cout << "No updates received in 15 seconds. Shutting down." << std::endl;
                break;  // Timeout reached and no new data, shut down federate
            }
        }

        // If new data has been received, reset lastUpdateTime
        if (receivedNewData) {
            lastUpdateTime = std::chrono::steady_clock::now();  // Update lastUpdateTime
            receivedNewData = false;  // Reset flag for the next loop cycle
            std::cout << "Received new data. Resetting timer." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Small delay for polling
    }
}

void Federate::finalize() {
    try {
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void Federate::resignFederation() {
    try {
        _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        _rtiAmbassador->destroyFederationExecution(L"ExampleFederation");
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}