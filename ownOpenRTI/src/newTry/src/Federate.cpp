#include "../include/Federate.h"
#include <iostream>

Federate::Federate() : _state(0), _rtiAmbassador(std::make_unique<rti1516e::RTIambassador>()), _federateAmbassador(std::make_unique<FederateAmbassador>()) {
}

Federate::~Federate() {
    finalize();
}

void Federate::initialize() {
    try {
        joinFederation();
        std::cout << "Federate initialized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
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

void Federate::joinFederation() {
    try {
        _rtiAmbassador->connect(*_federateAmbassador, rti1516e::HLA_EVOKED);
        _rtiAmbassador->createFederationExecution(L"ExampleFederation", L"ExampleFOM.xml");
        _rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation", _federateAmbassador.get());
        std::cout << "Joined federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::cerr << "Error joining federation: " << e.what() << std::endl;
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