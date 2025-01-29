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

void Federate::run() {
    std::cout << "Federate running." << std::endl;
    // Implement the main logic of the federate here
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