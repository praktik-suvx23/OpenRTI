#include "PyLink.h"

PyLink::PyLink() {
    createRTIambassador();
}

PyLink::~PyLink() {
    resignFederation();
}

void PyLink::runFederate() {
    try {
        connectToRTI();
        initializeFederation();
        joinFederation();
        initializeHandles();
        publishAttributes();
        subscribeAttributes();
        initializeTimeFactory();
        enableTimeManagement();
        socketsSetup();
        readyCheck();
        adminLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] runFederate - Exception: " << e.what() << std::endl;
    }
}

void PyLink::createRTIambassador() {
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        federateAmbassador = std::make_unique<PyLinkAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIambassador - Exception" << e.what() << std::endl;
    }
}

void PyLink::connectToRTI() {
    try {
        if (!rtiAmbassador) {
            std::wcerr << L"RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] connectToRTI - Exception: " << e.what() << std::endl;
    }
}

void PyLink::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecution(federationName, fomModules);
        std::wcout << L"[DEBUG] Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists& e) {
        std::wcout << L"[DEBUG] Federation already exists: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeFederation - Exception: " << e.what() << std::endl;
    }
}

void PyLink::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"[DEBUG] Joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] joinFederation - Exception: " << e.what() << std::endl;
    }
}

void PyLink::initializeHandles() {
    // Initialize handles for object classes and attributes
    // This is a placeholder, actual implementation will depend on the FOM
}

void PyLink::publishAttributes() {
    // Publish attributes for the federate
    // This is a placeholder, actual implementation will depend on the FOM
}

void PyLink::subscribeAttributes() {
    // Subscribe to attributes for the federate
    // This is a placeholder, actual implementation will depend on the FOM
}

void PyLink::initializeTimeFactory() {
    try {
        if(!rtiAmbassador) {
            std::wcerr << L"RTIambassador is null" << std::endl;
            exit(1);
        }

        auto factoryPtr = rtiAmbassador->getTimeFactory();
        logicalTimeFactory = dynamic_cast<rti1516e::HLAfloat64TimeFactory*>(factoryPtr.get());

        if (!logicalTimeFactory) {
            std::wcerr << L"[DEBUG] Failed to retrieve HLAfloat64TimeFactory from RTI." << std::endl;
            exit(1);
        }

        std::wcout << L"[DEBUG] HLAfloat64TimeFactory initialized: " 
                   << logicalTimeFactory->getName() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeTimeFactory - Exception: " << e.what() << std::endl;
    }
}

void PyLink::enableTimeManagement() {
    try {
        if (federateAmbassador->getIsRegulating()) {
            std::wcout << L"[DEBUG] Time Regulation already enabled. Skipping..." << std::endl;
            return;
        }
        auto lookahead = rti1516e::HLAfloat64Interval(0.5);  // Lookahead must be > 0
        std::wcout << L"[DEBUG] Enabling Time Management..." << std::endl;

        rtiAmbassador->enableTimeRegulation(lookahead);
        while (!federateAmbassador->getIsRegulating()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"[DEBUG] Time Regulation enabled." << std::endl;

        rtiAmbassador->enableTimeConstrained();
        while (!federateAmbassador->getIsConstrained()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"[DEBUG] Time Constrained enabled." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] enableTimeManagement - Exception: " << e.what() << std::endl;
    }
}

void PyLink::socketsSetup() {
    // Setup sockets for communication with Python
    // This is a placeholder, actual implementation will depend on the requirements
}

void PyLink::readyCheck() {
    try {
        while (federateAmbassador->getSyncLabel() != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[INFO] PyLink is ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] readyCheck - Exception: " << e.what() << std::endl;
    }
}

void PyLink::adminLoop() {
    // Main loop for the federate
    // This is a placeholder, actual implementation will depend on the requirements
}

void PyLink::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"[DEBUG] Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] resignFederation - Exception: " << e.what() << std::endl;
    }
}

int main() {
    PyLink pyLink;
    pyLink.runFederate();
    return 0;
}