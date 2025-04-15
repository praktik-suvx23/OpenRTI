#include "AdminFederate.h"
#include "AdminFederateAmbassadorHelper.h"
#include "AdminFederateFunctions.h"

#include <limits>

AdminFederate::AdminFederate() {
    createRTIambassador();
}

AdminFederate::~AdminFederate() {
    std::wcout << "Resigning federation in 15 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    resignFederation();
}

void AdminFederate::runFederate() {
    connectToRTI();
    initializeFederation();
    joinFederation();
    registerSyncPoint();
    achiveSyncPoint();
    initializeHandles();
    publishInteractions();
    setupSimulation();
    registerSyncSimulationSetupComplete();
    initializeTimeFactory();
    enableTimeManagement();
    socketsSetup();
    readyCheck();
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
        std::wcout << L"[ERROR] Unknown Exception in connectToRTI!" << std::endl;
    }
}

void AdminFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, minModule);
    } catch (const rti1516e::FederationExecutionAlreadyExists& e) {
        std::wcout << L"[DEBUG] Federation already exists: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionDoesNotExist& e) {
        std::wcout << L"[ERROR] Federation does not exist: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeFederation - Exception: " << e.what() << std::endl;
    } catch (...) {
        std::wcout << L"[ERROR] Unknown Exception in initializeFederation!" << std::endl;
    }
}

void AdminFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
    } catch (...) {
        std::wcout << L"[ERROR] Unknown Exception in joinFederation!" << std::endl;
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
        std::wcout << L"[ERROR] While registering synchronization point: " << e.what() << std::endl;
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
    // Create a socket for heartbeat communication
    heartbeat_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (heartbeat_socket < 0) {
        std::wcout << L"[ERROR] Failed to create heartbeat socket." << std::endl;
        return;
    }

    // Set up the sockaddr_in structure for the heartbeat port
    sockaddr_in heartbeat_addr{};
    heartbeat_addr.sin_family = AF_INET;
    heartbeat_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP
    heartbeat_addr.sin_port = htons(HEARTBEAT_PORT);  // Port 12348

    // Bind the socket to the heartbeat port
    if (bind(heartbeat_socket, (struct sockaddr*)&heartbeat_addr, sizeof(heartbeat_addr)) < 0) {
        std::cerr << "[ERROR] Failed to bind heartbeat socket to port " << HEARTBEAT_PORT << std::endl;
        close(heartbeat_socket);
        return;
    }

    // Start listening on the heartbeat socket
    if (listen(heartbeat_socket, 1) < 0) {
        std::cerr << "[ERROR] Listen failed on heartbeat socket" << std::endl;
        close(heartbeat_socket);
        return;
    }

    std::wcout << L"[INFO] AdminFederate is now listening on HEARTBEAT_PORT " << HEARTBEAT_PORT << "..." << std::endl;
}

void AdminFederate::readyCheck() {
    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"AdminReady", rti1516e::VariableLengthData());

        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"AdminReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"RedShipReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        rtiAmbassador->registerFederationSynchronizationPoint(L"EveryoneReady", rti1516e::VariableLengthData());
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"[INFO] All Federates are ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Exception during readyCheck: " << e.what() << std::endl;
        exit(1);
    }
}

void AdminFederate::adminLoop() {

    bool missileData, redshipData, blueshipData = false;
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double tmpTimeScale = timeScaleFactor;
    double remainingTime = {};
    auto startTime = std::chrono::high_resolution_clock::now();
    auto lastDataReceivedTime = std::chrono::high_resolution_clock::now();
    int heartPulse = 0;
    static int lastLoggedSecond = -1;


    if (!logicalTimeFactory) {
        std::wcerr << L"Logical time factory is null" << std::endl;
        exit(1);
    }

    while (true) {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        receiveHeartbeat = listenForHeartbeat(heartbeat_socket);

        auto stepEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedRealWorldTime = stepEndTime - startTime;

        double remainingTime = (stepsize/tmpTimeScale) - elapsedRealWorldTime.count();
        if (remainingTime > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(remainingTime));
        }

        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        if (heartPulse % 10 == 0) { // TODO: Make this based on time
            std::wcout << L"[PULSE] Admin loop is running..." << std::endl;
        }
        heartPulse++;


        if (receiveHeartbeat == 1) {  // 'alive'
            lastDataReceivedTime = std::chrono::high_resolution_clock::now();
            
        } else {
            if (receiveHeartbeat == 0) {  // 'complete'
                std::wcout << L"[INFO] Heartbeat complete. Exiting admin loop." << std::endl;
                break;
            } else if (receiveHeartbeat >= 2) {  // Error codes
                std::wcout << L"[ERROR] Heartbeat error code: " << receiveHeartbeat << std::endl;
            } else {
                std::wcout << L"[ERROR] Unknown error code: " << receiveHeartbeat << std::endl;
            }
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> timeSinceLastData = currentTime - lastDataReceivedTime;

            if (timeSinceLastData.count() > 15.0) {
                std::wcout << L"[INFO] No data available on any socket for a long time. Exiting..." << std::endl;
                break;
            }

            int currentSecond = static_cast<int>(timeSinceLastData.count());
            if (currentSecond % 5 == 0 && currentSecond != lastLoggedSecond) {
                std::wcout << L"[INFO] No data available on any socket for " 
                        << timeSinceLastData.count() << L" seconds." << std::endl;
                lastLoggedSecond = currentSecond;
            }
        }

        simulationTime += stepsize;
        receiveHeartbeat = -1;
        startTime = std::chrono::high_resolution_clock::now();
    }

    close(heartbeat_socket);

    rtiAmbassador->registerFederationSynchronizationPoint(L"ReadyToExit", rti1516e::VariableLengthData());

    while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"ReadyToExit") {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
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