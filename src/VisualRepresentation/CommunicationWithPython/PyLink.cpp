#include "PyLink.h"

PyLink::PyLink() {
    createRTIambassador();
}

PyLink::~PyLink() {
    logWmessage = L"[DESTRUCTOR] Program terminated.";
    std::wcout << logWmessage << std::endl;
    wstringToLog(logWmessage, federateAmbassador->getLogType());
    std::this_thread::sleep_for(std::chrono::seconds(10));
    close(redship_socket);
    close(blueship_socket);
    close(missile_socket);
    resignFederation();
}

void PyLink::runFederate() {
    federateAmbassador->setLogType(loggingType::LOGGING_PYLINK);
    initializeLogFile(federateAmbassador->getLogType());
    logToFile("[INFO] New run begun...", federateAmbassador->getLogType());
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
        std::wcout << L"[INFO] Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists& e) {
        std::wcout << L"[DEBUG] initializeFederation - Federation already exists: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeFederation - Exception: " << e.what() << std::endl;
    }
}

void PyLink::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"[INFO] Joined federation: " << federationName << std::endl;
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
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipID());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleShipSize());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"[INFO] Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] subscribeAttributes SHIP - Exception: " << e.what() << std::endl;
    }
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleMissileID());
        attributes.insert(federateAmbassador->getAttributeHandleMissileTeam());
        attributes.insert(federateAmbassador->getAttributeHandleMissilePosition());
        attributes.insert(federateAmbassador->getAttributeHandleMissileAltitude());
        attributes.insert(federateAmbassador->getAttributeHandleMissileSpeed());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleMissile(), attributes);
        std::wcout << L"[INFO] Subscribed to missile attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] subscribeAttributes MISSILE - Exception: " << e.what() << std::endl;
    }
}

void PyLink::initializeTimeFactory() {
    try {
        if(!rtiAmbassador) {
            std::wcerr << L"[DEBUG] RTIambassador is null" << std::endl;
            exit(1);
        }

        auto factoryPtr = rtiAmbassador->getTimeFactory();
        logicalTimeFactory = dynamic_cast<rti1516e::HLAfloat64TimeFactory*>(factoryPtr.get());

        if (!logicalTimeFactory) {
            std::wcerr << L"[DEBUG] Failed to retrieve HLAfloat64TimeFactory from RTI." << std::endl;
            exit(1);
        }

        std::wcout << L"[INFO] HLAfloat64TimeFactory initialized: " 
                   << logicalTimeFactory->getName() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeTimeFactory - Exception: " << e.what() << std::endl;
    }
}

void PyLink::enableTimeManagement() {
    try {
        if (federateAmbassador->getIsRegulating()) {
            std::wcout << L"[INFO] Time Regulation already enabled. Skipping..." << std::endl;
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

        // Set up the socket addresses for respective ships and missiles
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

auto socket_has_pending_data = [](int sock) {
    int outq = 0;
    if (ioctl(sock, TIOCOUTQ, &outq) == 0) {
        return outq > 0;
    }
    return false; // If ioctl fails, assume no data
};

void PyLink::communicationLoop() {
    federateAmbassador->setStartTime(std::chrono::high_resolution_clock::now());
    double simulationTime = 0.0;
    const double stepsize = 0.5;
    int pulse = 0;
    int sendInterval = 10;      // Send updates every 10 iterations

    uint8_t blueNoMessageCounter = 0;
    uint8_t redNoMessageCounter = 0;
    uint8_t missileNoMessageCounter = 0;

    auto& missiles = federateAmbassador->getMissiles();
    auto& blueShips = federateAmbassador->getBlueShips();
    auto& redShips = federateAmbassador->getRedShips();

    if(!logicalTimeFactory) {
        std::wcerr << L"[DEBUG] logicalTimeFactory is null" << std::endl;
        exit(1);
    }

    while (federateAmbassador->getSyncLabel() != L"ReadyToExit" && federateAmbassador->getSyncLabel() != L"NoMessagesReceived") {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - federateAmbassador->getStartTime()
        ).count();

        // Blue ships
        if (blueShips.empty()) {
            blueNoMessageCounter++;
        } else {
            blueNoMessageCounter = 0;
            for (auto it = blueShips.begin(); it != blueShips.end(); ) {
                if (shipUpdateCount[it->shipName] % sendInterval == 0) {
                    send_ship(blueship_socket, *it);
                    logShip(*it);
                    it = blueShips.erase(it);
                } else {
                    shipUpdateCount[it->shipName]++;
                    ++it;
                }
            }
        }

        // Red ships
        if (redShips.empty()) {
            redNoMessageCounter++;
        } else {
            redNoMessageCounter = 0;
            for (auto it = redShips.begin(); it != redShips.end(); ) {
                if (shipUpdateCount[it->shipName] % sendInterval == 0) {
                    send_ship(redship_socket, *it);
                    logShip(*it);
                    it = redShips.erase(it);
                } else {
                    shipUpdateCount[it->shipName]++;
                    ++it;
                }
            }
        }

        // Missiles
        if (missiles.empty()) {
            missileNoMessageCounter++;
        } else {
            missileNoMessageCounter = 0;
            for (auto it = missiles.begin(); it != missiles.end(); ) {
                if (missileUpdateCount[it->id] % sendInterval == 0) {
                    send_missile(missile_socket, *it);
                    logMissile(*it);
                    it = missiles.erase(it);
                } else {
                    missileUpdateCount[it->id]++;
                    ++it;
                }
            }
        }

        if (elapsedTime / 10 > pulse) {
            pulse = elapsedTime / 10;
            std::wcout << L"[PULSE] Elapsed time: " << elapsedTime << L" seconds." << std::endl;
        }

        if (blueNoMessageCounter > 20 && redNoMessageCounter > 20 && missileNoMessageCounter > 20) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"NoMessagesReceived", rti1516e::VariableLengthData());
            while (federateAmbassador->getSyncLabel() != L"NoMessagesReceived") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }

        federateAmbassador->setIsAdvancing(true);
        rtiAmbassador->timeAdvanceRequest(logicalTime);
        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SIMULATION TIME << " << simulationTime << L"]" << std::endl;
        simulationTime += stepsize;
    }
    while (
        socket_has_pending_data(redship_socket) ||
        socket_has_pending_data(blueship_socket) ||
        socket_has_pending_data(missile_socket)
    ) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void PyLink::logShip(Ship& ship) {
    logWmessage = L"[SENT DATA] Ship: " + ship.shipName + L" | Team: " + ship.shipTeam
        + L" | Position: (" + std::to_wstring(ship.shipPosition.first) + L", " +
        std::to_wstring(ship.shipPosition.second) + L") | Speed: " + std::to_wstring(ship.shipSpeed);
    wstringToLog(logWmessage, federateAmbassador->getLogType());
}

void PyLink::logMissile(Missile& missile) {
    logWmessage = L"[SENT DATA] Missile: " + missile.id + L" | Team: " + missile.team
        + L" | Position: (" + std::to_wstring(missile.position.first) + L", " +
        std::to_wstring(missile.position.second) + L") | Speed: " + std::to_wstring(missile.speed);
    wstringToLog(logWmessage, federateAmbassador->getLogType());
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