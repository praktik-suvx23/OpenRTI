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
        waitForSyncPoint();
        initializeHandles();
        subscribeAttributes();
        initializeTimeFactory();
        enableTimeManagement();
        socketsSetup();
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

void PyLink::waitForSyncPoint() {
    std::wcout << L"[INFO] Waiting for \'InitialSync\' sync point..." << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
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

        federateAmbassador->setObjectClassHandleMissile(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Missile"));
        federateAmbassador->setAttributeHandleMissileID(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileID"));
        federateAmbassador->setAttributeHandleMissileTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileTeam"));
        federateAmbassador->setAttributeHandleMissilePosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Position"));
        federateAmbassador->setAttributeHandleMissileAltitude(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Altitude"));
        federateAmbassador->setAttributeHandleMissileSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Speed"));
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeHandles - Exception: " << e.what() << std::endl;
    }
}

void PyLink::subscribeAttributes() {
    rti1516e::AttributeHandleSet attributes;
    try {
        attributes.insert(federateAmbassador->getAttributeHandleShipID());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleShipSize());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"[DEBUG] Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] subscribeAttributes - Exception: " << e.what() << std::endl;
    }
    try {
        attributes.insert(federateAmbassador->getAttributeHandleMissileID());
        attributes.insert(federateAmbassador->getAttributeHandleMissileTeam());
        attributes.insert(federateAmbassador->getAttributeHandleMissilePosition());
        attributes.insert(federateAmbassador->getAttributeHandleMissileAltitude());
        attributes.insert(federateAmbassador->getAttributeHandleMissileSpeed());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleMissile(), attributes);
        std::wcout << L"[DEBUG] Subscribed to missile attributes" << std::endl;
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
        auto lookahead = rti1516e::HLAfloat64Interval(0.5);
        std::wcout << L"[INFO] Enabling Time Management... ";

        rtiAmbassador->enableTimeRegulation(lookahead);
        while (!federateAmbassador->getIsRegulating()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        rtiAmbassador->enableTimeConstrained();
        while (!federateAmbassador->getIsConstrained()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"|| Successfully enabled Time Management." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"[DEBUG] enableTimeManagement - Exception: " << e.what() << std::endl;
    }
}

void PyLink::socketsSetup() {
    try {
        // Setup sockets for communication with Python
        redship_socket = socket(AF_INET, SOCK_STREAM, 0);
        blueship_socket = socket(AF_INET, SOCK_STREAM, 0);
        missile_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (redship_socket < 0 || blueship_socket < 0 || missile_socket < 0) {
            std::wcout << L"[DEBUG] Failed to create socket." << std::endl;
            return;
        }

        // Set up the socket addresses for red and blue ships
        sockaddr_in redship_addr, blueship_addr, missile_addr;
        redship_addr.sin_family = AF_INET;
        redship_addr.sin_addr.s_addr = INADDR_ANY;
        redship_addr.sin_port = htons(REDSHIP_PORT);
        blueship_addr.sin_family = AF_INET;
        blueship_addr.sin_addr.s_addr = INADDR_ANY;
        blueship_addr.sin_port = htons(BLUESHIP_PORT);
        missile_addr.sin_family = AF_INET;
        missile_addr.sin_addr.s_addr = INADDR_ANY;
        missile_addr.sin_port = htons(MISSILE_PORT);

        // Bind the sockets to the respective ports
        if(connect(redship_socket, (struct sockaddr*)&redship_addr, sizeof(redship_addr)) < 0) {
            std::wcout << L"[DEBUG] Failed to connect redship socket." << std::endl;
            close(redship_socket);
            return;
        }
        if(connect(blueship_socket, (struct sockaddr*)&blueship_addr, sizeof(blueship_addr)) < 0) {
            std::wcout << L"[DEBUG] Failed to connect blueship socket." << std::endl;
            close(blueship_socket);
            return;
        }
        if(connect(missile_socket, (struct sockaddr*)&missile_addr, sizeof(missile_addr)) < 0) {
            std::wcout << L"[DEBUG] Failed to connect missile socket." << std::endl;
            close(missile_socket);
            return;
        }
        std::wcout << L"[INFO] Sockets connected to ports:" 
                   << L" Redship: " << REDSHIP_PORT 
                   << L" Blueship: " << BLUESHIP_PORT 
                   << L" Missile: " << MISSILE_PORT << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"[DEBUG] socketsSetup - Exception: " << e.what() << std::endl;
        exit(1);
    }
}

void PyLink::readyCheck() {
    std::wcout << L"[INFO] Waiting to start. Requesting synchronization point..." << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] readyCheck - Exception: " << e.what() << std::endl;
    }
}

void PyLink::communicationLoop() {
    federateAmbassador->setStartTime(std::chrono::high_resolution_clock::now());
    double simulationTime = 0.0;
    const double stepsize = 0.5;
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
            std::wcout << L"[PULSE] Elapsed time: " << elapsedTime << L" seconds." << std::endl;
        }

        for (auto& blueShip : federateAmbassador->getBlueShips()) {
            send_ship(blueship_socket, blueShip);
        }
        for (auto& redShip : federateAmbassador->getRedShips()) {
            send_ship(redship_socket, redShip);
        }
        for (auto& missile : federateAmbassador->getMissiles()) {
            send_missile(missile_socket, missile);
        }
        federateAmbassador->getBlueShips().clear();
        federateAmbassador->getRedShips().clear();
        federateAmbassador->getMissiles().clear();

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
        std::wcout << L"[INFO] Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] resignFederation - Exception: " << e.what() << std::endl;
    }
}

int main() {
    PyLink pyLink;
    pyLink.runFederate();
    return 0;
}