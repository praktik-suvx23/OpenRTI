#include "AdminFederate.h"
#include "AdminFederateAmbassadorHelper.h"
#include "AdminFederateFunctions.h"

#include <limits>

AdminFederate::AdminFederate() {
    createRTIambassador();
}

AdminFederate::~AdminFederate() {
    resignFederation();
}

void AdminFederate::runFederate() {
    std::wcout << "Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
    connectToRTI();

    std::wcout << "Creating federation..." << std::endl;
    initializeFederation();

    std::wcout << "Joining federation..." << std::endl;
    joinFederation();

    std::wcout << "Registering sync point..." << std::endl;
    registerSyncPoint();

    std::wcout << "Achieving sync point..." << std::endl;
    achiveSyncPoint();

    std::wcout << "Initializing handles..." << std::endl;
    initializeHandles();

    std::wcout << "Publishing interactions..." << std::endl;
    publishInteractions();

    std::wcout << "Setup simulation..." << std::endl;
    setupSimulation();

    std::wcout << "Registering sync point for simulation setup complete..." << std::endl;
    registerSyncSimulationSetupComplete();

    std::wcout << "Initializing time factory..." << std::endl;
    initializeTimeFactory();

    std::wcout << "Enabling time management..." << std::endl;
    enableTimeManagement();

    std::wcout << "Sockets setup..." << std::endl;
    socketsSetup();

    std::wcout << "Ready check..." << std::endl;
    readyCheck();

    std::wcout << "Admin loop..." << std::endl;
    adminLoop();
}

void AdminFederate::createRTIambassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<AdminFederateAmbassador>(rtiAmbassador.get());
}

void AdminFederate::connectToRTI() {
    try{
        if(!rtiAmbassador){
            std::cout << "RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    }catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, minModule);
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::registerSyncPoint() {
    std::wcout << "Press 'Enter' to register synchronization point..." << std::endl;
    userInteraction();

    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"InitialSync", rti1516e::VariableLengthData());
        std::wcout << L"Sync Federate waiting for synchronization..." << std::endl;
    
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"Sync Federate has announced synchronization point: InitialSync" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while registering synchronization point: " << e.what() << std::endl;
    }
}

void AdminFederate::achiveSyncPoint() {    
    try {
        rtiAmbassador->synchronizationPointAchieved(L"InitialSync", true);
        std::wcout << "Synchronization point 'InitialSync' achieved!" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while achieving synchronization point: " << e.what() << std::endl;
    }
}

void AdminFederate::initializeHandles() {
    try {
        AmbassadorSetter::setSetupSimulationHandle(*federateAmbassador, rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
        AmbassadorSetter::setBlueShipsParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"NumberOfBlueShips"));
        AmbassadorSetter::setRedShipsParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"NumberOfRedShips"));
        AmbassadorSetter::setTimeScaleFactorParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"TimeScaleFactor"));
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error initializing handles: " << e.what() << std::endl;
    }
}

void AdminFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador));
        std::wcout << "Published SetupSimulation interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing SetupSimulation interaction: " << e.what() << std::endl;
    }
}

void AdminFederate::setupSimulation() {
    /* Improve this. This isn't OK. Just POC. Need better 'wcin' handling.
    Also: Actually use this in the different ship federates. */
    std::wcout << "Enter initial amount of sips on team 'Blue': " << std::endl;
    blueShips = getValidIntInput();

    std::wcout << "Enter initial amount of sips on team 'Red': " << std::endl;
    redShips = getValidIntInput();

    std::wcout << "Enter time scale factor: " << std::endl;
    timeScaleFactor = getValidDoubleInput();

    publishSetupSimulationInteraction(blueShips, redShips, timeScaleFactor);
}

void AdminFederate::publishSetupSimulationInteraction(int teamA, int teamB, double timeScaleFactor) {
    rti1516e::ParameterHandleValueMap parameters;
    parameters[AmbassadorGetter::getBlueShipsParam(*federateAmbassador)] = rti1516e::HLAinteger32BE(teamA).encode();
    parameters[AmbassadorGetter::getRedShipsParam(*federateAmbassador)] = rti1516e::HLAinteger32BE(teamB).encode();
    parameters[AmbassadorGetter::getTimeScaleFactorParam(*federateAmbassador)] = rti1516e::HLAfloat64BE(timeScaleFactor).encode();

    try {
        rtiAmbassador->sendInteraction(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), parameters, rti1516e::VariableLengthData());
        std::wcout << "Sent SetupSimulation interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error sending SetupSimulation interaction: " << e.what() << std::endl;
    }
}

void AdminFederate::registerSyncSimulationSetupComplete() {
    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"SimulationSetupComplete", rti1516e::VariableLengthData());
        std::wcout << L"Registered sync point: SimulationSetupComplete" << std::endl;
        
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }    
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"Error registering SimulationSetupComplete sync point: " << e.what() << std::endl;
    }
}

void AdminFederate::initializeTimeFactory() {
    try {
        if (!rtiAmbassador) {
            throw std::runtime_error("rtiAmbassador is NULL! Cannot retrieve time factory.");
        }

        auto factoryPtr = rtiAmbassador->getTimeFactory();
        logicalTimeFactory = dynamic_cast<rti1516e::HLAfloat64TimeFactory*>(factoryPtr.get());

        if (!logicalTimeFactory) {
            throw std::runtime_error("Failed to retrieve HLAfloat64TimeFactory from RTI.");
        }

        std::wcout << L"[SUCCESS] HLAfloat64TimeFactory initialized: " 
                   << logicalTimeFactory->getName() << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"[ERROR] Exception in initializeTimeFactory: " << e.what() << std::endl;
    }
}

void AdminFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
    try {
        if (federateAmbassador->isRegulating) {  // Prevent enabling twice
            std::wcout << L"[WARNING] Time Regulation already enabled. Skipping..." << std::endl;
            return;
        }
        /*
        Lookahead is the minimum amount of time the federate can look into the future
        and makes sure that the logical time must advance by at least this amount before 
        it can send an event or update attributes.
        */
        auto lookahead = rti1516e::HLAfloat64Interval(0.5);  // Lookahead must be > 0
        std::wcout << L"[INFO] Enabling Time Management..." << std::endl;
        
        rtiAmbassador->enableTimeRegulation(lookahead);
        while (!federateAmbassador->isRegulating) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS] Time Regulation enabled." << std::endl;

        rtiAmbassador->enableTimeConstrained();
        while (!federateAmbassador->isConstrained) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS] Time Constrained enabled." << std::endl;

    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"[ERROR] Exception during enableTimeManagement: " << e.what() << std::endl;
    }
}

void AdminFederate::socketsSetup() {
    missile_socket = socket(AF_INET, SOCK_STREAM, 0);
    redship_socket = socket(AF_INET, SOCK_STREAM, 0);
    blueship_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (missile_socket < 0 || redship_socket < 0 || blueship_socket < 0) {
        std::wcout << L"[ERROR] Failed to create sockets." << std::endl;
        return;
    }

    // Bind sockets to their respective ports
    sockaddr_in missile_addr{}, redship_addr{}, blueship_addr{};
    missile_addr.sin_family = AF_INET;
    missile_addr.sin_addr.s_addr = INADDR_ANY;
    missile_addr.sin_port = htons(MISSILE_PORT);

    redship_addr.sin_family = AF_INET;
    redship_addr.sin_addr.s_addr = INADDR_ANY;
    redship_addr.sin_port = htons(REDSHIP_PORT);

    blueship_addr.sin_family = AF_INET;
    blueship_addr.sin_addr.s_addr = INADDR_ANY;
    blueship_addr.sin_port = htons(BLUESHIP_PORT);

    if (bind(missile_socket, (struct sockaddr*)&missile_addr, sizeof(missile_addr)) < 0 ||
        bind(redship_socket, (struct sockaddr*)&redship_addr, sizeof(redship_addr)) < 0 ||
        bind(blueship_socket, (struct sockaddr*)&blueship_addr, sizeof(blueship_addr)) < 0) {
        std::cerr << "[ERROR] Failed to bind sockets to ports." << std::endl;
        close(missile_socket);
        close(redship_socket);
        close(blueship_socket);
        return;
    }

    std::wcout << L"[INFO] AdminFederate is now listening on ports." << std::endl;
}

void AdminFederate::readyCheck() {
    try {
        std::wcout << "[DEBUG] 1" << std::endl;
        rtiAmbassador->registerFederationSynchronizationPoint(L"AdminReady", rti1516e::VariableLengthData());

        std::wcout << "[DEBUG] 2" << std::endl;
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"AdminReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << "[DEBUG] 3" << std::endl;
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"RedShipReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        rtiAmbassador->registerFederationSynchronizationPoint(L"EveryoneReady", rti1516e::VariableLengthData());
        std::wcout << "[DEBUG] 4" << std::endl;
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"[INFO] All ships are ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Exception during readyCheck: " << e.what() << std::endl;
        exit(1);
    }
}

void AdminFederate::adminLoop() {
    std::wcout << "~ Bye bye ~\nTODO: Do something with AdminFederate.cpp - adminLoop" << std::endl;

    bool missileData, redshipData, blueshipData = false;
    double simulationTime = 0.0;
    double stepsize = 0.5;

    if (!logicalTimeFactory) {
        std::wcerr << L"Logical time factory is null" << std::endl;
        exit(1);
    }

    while (true) {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        missileData = isSocketTransmittingData(missile_socket);
        redshipData = isSocketTransmittingData(redship_socket);
        blueshipData = isSocketTransmittingData(blueship_socket);

        if (!missileData && !redshipData && !blueshipData) {
            std::wcout << L"[INFO] No data available on any socket." << std::endl;
            break;
        }

        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        simulationTime += stepsize;
    }

    close(missile_socket);
    close(redship_socket);
    close(blueship_socket);
    /* TODO: Implement admin loop functionality
    For example:
    - Determine when to end the simulation
    - Send interactions to control the simulation
    - Manage removal of entities
    - Manage communication between ships
    - Manage which ships fire at which ships
    - Manage which ships are hit
    - Manage adding new ships
    etc.
    */
}

void AdminFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    try {
        AdminFederate myFederate;
        myFederate.runFederate();
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
    
    return 0;
}