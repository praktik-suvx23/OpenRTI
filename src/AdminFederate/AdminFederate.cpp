#include "AdminFederate.h"
#include "AdminFederateAmbassadorHelper.h"
#include "AdminFederateFunctions.h"

#include <limits>
#include <algorithm>

AdminFederate::AdminFederate() {
    createRTIambassador();
}

AdminFederate::~AdminFederate() {
    int sleepTime = 5;
    std::wcout << "Resigning federation in " << sleepTime << " seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
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
    subscribeInteractions();
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
        // ===== INITIATE RED =====
        federateAmbassador->setInteractionClassInitiateRedHandshake(
            rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeRed"));
        federateAmbassador->setParamInitiateRedShooterID(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"ShooterID"));
        federateAmbassador->setParamInitiateRedMissilesAvailable(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"ShooterMissilesAvailable"));
        federateAmbassador->setParamInitiateRedTargetID(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"TargetID"));
        federateAmbassador->setParamInitiateRedMaxMissilesRequired(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"TargetMaxMissilesRequired"));
        federateAmbassador->setParamInitiateRedMissilesCurrentlyTargeting(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"TargetMissilesCurrentlyTargeting"));
        federateAmbassador->setParamInitiateRedDistanceToTarget(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateRedHandshake(), L"DistanceToTarget"));

        std::wcout << L"InteractionClassInitiateRedHandshake initialized: " 
                << federateAmbassador->getInteractionClassInitiateRedHandshake() << std::endl;

        // ===== INITIATE BLUE =====
        federateAmbassador->setInteractionClassInitiateBlueHandshake(
            rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeBlue"));
        federateAmbassador->setParamInitiateBlueShooterID(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"ShooterID"));
        federateAmbassador->setParamInitiateBlueMissilesAvailable(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"ShooterMissilesAvailable"));
        federateAmbassador->setParamInitiateBlueTargetID(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"TargetID"));
        federateAmbassador->setParamInitiateBlueMaxMissilesRequired(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"TargetMaxMissilesRequired"));
        federateAmbassador->setParamInitiateBlueMissilesCurrentlyTargeting(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"TargetMissilesCurrentlyTargeting"));
        federateAmbassador->setParamInitiateBlueDistanceToTarget(
            rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassInitiateBlueHandshake(), L"DistanceToTarget"));

        std::wcout << L"InteractionClassInitiateBlueHandshake initialized: " 
                << federateAmbassador->getInteractionClassInitiateBlueHandshake() << std::endl;

        for (Team side : {Team::Red, Team::Blue}) {
            std::wstring interactionName = (side == Team::Red)
                ? L"HLAinteractionRoot.ConfirmHandshakeRed"
                : L"HLAinteractionRoot.ConfirmHandshakeBlue";
        
            // Set the interaction class handle
            rti1516e::InteractionClassHandle interactionHandle = rtiAmbassador->getInteractionClassHandle(interactionName);
            federateAmbassador->setInteractionClassConfirmHandshake(side, interactionHandle);
        
            // Set the parameter handles
            federateAmbassador->setParamConfirmShooterID(
                side, rtiAmbassador->getParameterHandle(interactionHandle, L"ShooterID"));
            federateAmbassador->setParamConfirmMissilesLoaded(
                side, rtiAmbassador->getParameterHandle(interactionHandle, L"ShooterMissilesLoaded"));
            federateAmbassador->setParamConfirmTargetID(
                side, rtiAmbassador->getParameterHandle(interactionHandle, L"TargetID"));
            federateAmbassador->setParamConfirmBoolean(
                side, rtiAmbassador->getParameterHandle(interactionHandle, L"AllowedFire"));
        
            std::wcout << L"InteractionClassConfirmHandshake initialized for "
                        << ((side == Team::Blue) ? L"BLUE" : L"RED")
                        << L": " << interactionHandle << std::endl;
        }
        
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
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassConfirmHandshake(Team::Red));
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassConfirmHandshake(Team::Blue));
        rtiAmbassador->publishInteractionClass(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador));
        std::wcout << "Published SetupSimulation interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing SetupSimulation interaction: " << e.what() << std::endl;
    }
}

void AdminFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassInitiateRedHandshake());
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassInitiateBlueHandshake());
        std::wcout << "Subscribed to interactions." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error subscribing to interactions: " << e.what() << std::endl;
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
    try {
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
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"[ERROR] Exception during socket setup: " << e.what() << std::endl;
    } catch (...) {
        std::wcout << L"[ERROR] Unknown exception during socket setup." << std::endl;
    }
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
    bool connection = false;
    bool shouldCheckHeartbeat = true;
    int lastPulseTick = 0;
    int lastHeartbeatCode = 99;
    double currentSimTime = 0.0;
    const double simStepSize = 0.5;
    const double timeScale = timeScaleFactor;
    double sleepDuration = {};
    auto lastHeartbeatTime = std::chrono::high_resolution_clock::now();
    const auto startTime = lastHeartbeatTime;


    if (!logicalTimeFactory) {
        std::wcerr << L"Logical time factory is null" << std::endl;
        exit(1);
    }
    
    while (true) {
        federateAmbassador->setStartTime(std::chrono::high_resolution_clock::now());
        const rti1516e::HLAfloat64Time logicalTime(currentSimTime + simStepSize);
        if (shouldCheckHeartbeat) {
            HeartbeatStatus receiveHeartbeat = listenForHeartbeat(heartbeat_socket);
            if (!processHeartbeat(receiveHeartbeat, shouldCheckHeartbeat, lastHeartbeatCode, lastHeartbeatTime, connection)) {
                break;
            }
        }

        const auto stepEndTime = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> elapsedRealWorldTime = stepEndTime - federateAmbassador->getStartTime();

        double sleepDuration = (simStepSize/timeScale) - elapsedRealWorldTime.count();
        if (sleepDuration > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepDuration));
        }

        std::wcout << L"[Blue] ";
        flushInitialHandshake(federateAmbassador->getInitialHandshakeBlue(), blueConfirmHandshakeVector, federateAmbassador->getMissilesLeftToTargetBlue());
        std::wcout << L"[Red] ";
        flushInitialHandshake(federateAmbassador->getInitialHandshakeRed(), redConfirmHandshakeVector, federateAmbassador->getMissilesLeftToTargetRed());

        flushConfirmHandshake(blueConfirmHandshakeVector, Team::Blue);
        flushConfirmHandshake(redConfirmHandshakeVector, Team::Red);

        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        const auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - startTime);
    
        if (elapsedTime.count() / 10 > lastPulseTick) {
            lastPulseTick = elapsedTime.count() / 10;
            std::wcout << L"[PULSE] Elapsed time: " << elapsedTime.count() << L" seconds." << std::endl;
        }

        currentSimTime += simStepSize;
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

void AdminFederate::flushInitialHandshake(std::unordered_map<InitialHandshake, bool>& initialMap,
    std::vector<ConfirmHandshake>& confirmVector,
    std::unordered_map<std::wstring, std::optional<int32_t>>& missilesLeftToTarget) {
    std::wcout << L"Flushing initial handshake... InitialMap size: " << initialMap.size() << std::endl;
    if (!initialMap.empty()) {
        processInitialHandshake(
            initialMap,
            confirmVector,
            missilesLeftToTarget
        );
        initialMap.clear();
    }
}

void AdminFederate::processInitialHandshake(
    std::unordered_map<InitialHandshake, bool>& initialMap,
    std::vector<ConfirmHandshake>& confirmVector,
    std::unordered_map<std::wstring, std::optional<int32_t>>& missilesLeftToTarget)
{
    // Step 1: Group valid handshakes by target
    std::unordered_map<std::wstring, std::vector<InitialHandshake>> groupedByTarget;

    for (const auto& [hs, isActive] : initialMap) {
        if (!isActive) continue;

        auto it = missilesLeftToTarget.find(hs.targetID);
        if (it != missilesLeftToTarget.end() && it->second.has_value() && it->second.value() > 0) {
            groupedByTarget[hs.targetID].push_back(hs);
        }
    }

    // Step 2: Process each target group
    for (auto& [targetID, shooters] : groupedByTarget) {
        // Sort shooters by distance
        std::sort(shooters.begin(), shooters.end(), [](const InitialHandshake& a, const InitialHandshake& b) {
            return a.distanceToTarget < b.distanceToTarget;
        });

        int32_t missilesRemaining = missilesLeftToTarget[targetID].value_or(0);

        for (const auto& shooter : shooters) {
            if (missilesRemaining <= 0) break;

            int32_t canFire = std::min(missilesRemaining, shooter.missilesLoaded);

            if (canFire > 0) {
                ConfirmHandshake confirm {
                    shooter.shooterID,
                    shooter.missilesLoaded,
                    true,
                    shooter.targetID,
                    canFire
                };

                confirmVector.push_back(confirm);
                missilesRemaining -= canFire;
            }
        }

        // Update the remaining missile count
        missilesLeftToTarget[targetID] = std::max(0, missilesRemaining);
    }
    std::wcout << L"Processed initial handshakes. ConfirmVector size: " << confirmVector.size() << std::endl;
}

void AdminFederate::flushConfirmHandshake(std::vector<ConfirmHandshake>& vector, Team side) {
    if (!vector.empty()) {
        processConfirmHandshake(vector, side);
        vector.clear();
    }
}

void AdminFederate::processConfirmHandshake(
    std::vector<ConfirmHandshake>& confirmVector,
    Team side)
{
    for (const auto& confirm : confirmVector) {
        rti1516e::ParameterHandleValueMap parameters;

        parameters[federateAmbassador->getParamConfirmShooterID(side)] = rti1516e::HLAunicodeString(confirm.shooterID).encode();
        parameters[federateAmbassador->getParamConfirmMissilesLoaded(side)] = rti1516e::HLAinteger32BE(confirm.missilesLoaded).encode();
        parameters[federateAmbassador->getParamConfirmTargetID(side)] = rti1516e::HLAunicodeString(confirm.targetID).encode();
        parameters[federateAmbassador->getParamConfirmBoolean(side)] = rti1516e::HLAboolean(confirm.shooterReceivedOrder).encode();

        try {
            rtiAmbassador->sendInteraction(
                federateAmbassador->getInteractionClassConfirmHandshake(side),
                parameters,
                rti1516e::VariableLengthData());

            std::wcout << L"Sent ConfirmHandshake interaction (" 
                       << (side == Team::Blue ? L"Blue" : L"Red") 
                       << L") for target: " << confirm.targetID << std::endl;

        } catch (const rti1516e::Exception& e) {
            std::wcout << L"Error sending ConfirmHandshake interaction: " << e.what() << std::endl;
        }
    }
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