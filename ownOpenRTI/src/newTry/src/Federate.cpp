#include "../include/Federate.h"
#include <iostream>

Federate::Federate() : _state(0), _rtiAmbassador(std::make_unique<rti1516e::RTIambassador>()), _federateAmbassador(std::make_unique<FederateAmbassador>()) {
}

Federate::~Federate() {
    finalize();
}

void Federate::runFederate(const std::wstring& federateName) {
    this->fedAmb = new FederateAmbassador();
    try{
        _rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED);
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
    initialize();
    run();
    finalize();
}

void Federate::initialize() {
    try{
        _rtiAmbassador->createFederationExecution(L"ExampleFederation", L"foms/FOM.xml");
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch( Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }

    try{
        _rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation", _federateAmbassador.get());
    } catch (rti1516e::Exception& e){
        std::wcout << L"Error joining federation: " << e.what() << std::endl;
    }
}

void Federate::run() {
    std::cout << "Federate running." << std::endl;
    // Implement the main logic of the federate here
}

void Federate::finalize() {
    try {
        resignFederation();
        std::cout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error during finalization: " << e.what() << std::endl;
    }
}



void Federate::resignFederation() {
    try {
        _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        _rtiAmbassador->destroyFederationExecution(L"ExampleFederation");
        std::cout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error resigning from federation: " << e.what() << std::endl;
    }
}