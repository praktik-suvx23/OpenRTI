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
        subscribeAttributes();
        initializeTimeFactory();
        enableTimeManagement();
        //socketsSetup();   // Add me when ready
        readyCheck();
        communicationLoop();
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
    try {
        federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleShipID(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        //federateAmbassador->setAttributeHandleShipHP(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"HP"));
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeHandles - Exception: " << e.what() << std::endl;
    }
}

void PyLink::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipID());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleShipSize());
        //attributes.insert(federateAmbassador->getAttributeHandleShipHP());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"[DEBUG] Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] subscribeAttributes - Exception: " << e.what() << std::endl;
    }
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
    try {
        // Setup sockets for communication with Python
        redship_socket = socket(AF_INET, SOCK_STREAM, 0);
        blueship_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (redship_socket < 0 || blueship_socket < 0) {
            std::wcerr << L"[DEBUG] Failed to create socket." << std::endl;
            return;
        }

        // Set up the socket addresses for red and blue ships
        sockaddr_in redship_addr, blueship_addr;
        redship_addr.sin_family = AF_INET;
        redship_addr.sin_addr.s_addr = INADDR_ANY;
        redship_addr.sin_port = htons(REDSHIP_PORT);
        blueship_addr.sin_family = AF_INET;
        blueship_addr.sin_addr.s_addr = INADDR_ANY;
        blueship_addr.sin_port = htons(BLUESHIP_PORT);

        // Bind the sockets to the respective ports
        if(bind(redship_socket, (struct sockaddr*)&redship_addr, sizeof(redship_addr)) < 0) {
            std::wcerr << L"[DEBUG] Failed to bind redship socket." << std::endl;
            close(redship_socket);
            return;
        }
        if(bind(blueship_socket, (struct sockaddr*)&blueship_addr, sizeof(blueship_addr)) < 0) {
            std::wcerr << L"[DEBUG] Failed to bind blueship socket." << std::endl;
            close(blueship_socket);
            return;
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] socketsSetup - Exception: " << e.what() << std::endl;
        exit(1);
    }
}

void PyLink::readyCheck() {
    std::wcout << L"[INFO] Waiting to start. Requesting synchronization point..." << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[INFO] PyLink is ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] readyCheck - Exception: " << e.what() << std::endl;
    }
}

void PyLink::communicationLoop() {
    federateAmbassador->setStartTime(std::chrono::high_resolution_clock::now());
    double simulationTime = 0.0;
    double stepsize = 0.5;
    int pulse = 0;

    if(!logicalTimeFactory) {
        std::wcerr << L"[DEBUG] logicalTimeFactory is null" << std::endl;
        exit(1);
    }

    while (federateAmbassador->getSyncLabel() != L"ReadyToExit") {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - federateAmbassador->getStartTime()
        ).count();

        if (elapsedTime / 10 > pulse) {
            pulse = elapsedTime / 10;
            std::wcout << L"[PULSE] Elapsed time: " << elapsedTime << L" seconds. Pulse: " << pulse << std::endl;
        }

        for (auto& blueShip : federateAmbassador->getBlueShips()) {
            //std::wcout << L"[DEBUG] Blue ship: " << blueShip.objectInstanceHandle << std::endl; // Remove when working
            //if (blueship_socket > 0) {
            //    send_ship(blueship_socket, blueShip); // Add me when ready
            //}
        }
        for (auto& redShip : federateAmbassador->getRedShips()) {
            //std::wcout << L"[DEBUG] Red ship: " << redShip.objectInstanceHandle << std::endl;   // Remove when working
            //if (redship_socket > 0) {
            //    send_ship(redship_socket, redShip);   // Add me when ready
            //}
        }
        federateAmbassador->clearBlueShips();
        federateAmbassador->clearRedShips();

        federateAmbassador->setIsAdvancing(true);
        rtiAmbassador->timeAdvanceRequest(logicalTime);
        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        simulationTime += stepsize;
    }
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