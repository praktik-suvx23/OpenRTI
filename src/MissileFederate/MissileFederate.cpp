#include "MissileFederate.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

MissileFederate::MissileFederate() 
: gen(rd()), speedDis(250.0, 350.0) {
    createRTIAmbassador();
}

MissileFederate::~MissileFederate() {
    resignFederation();
}

void MissileFederate::startMissileManager() {
    connectToRTI();
    joinFederation();
    initializeHandles();
    subscribeAttributes();
    publishAttributes();
    subscribeInteractions();
    publishInteractions();
    setupMissileVisualization();
    initializeTimeFactory();
    enableTimeManagement();
    readyCheck();
    runSimulationLoop();
}

void MissileFederate::createRTIAmbassador() {
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        federateAmbassador = std::make_unique<MissileFederateAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIAmbassador - Exception" << e.what() << std::endl;
    }
}

void MissileFederate::connectToRTI() {
    try {
        if(!rtiAmbassador) {
            std::wcerr << L"[ERROR - connectToRTI] RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"[DEBUG] connectToRTI - Exception" << e.what() << std::endl;
    }
}

void MissileFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"[INFO] Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"[INFO] Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"[DEBUG - initializeFederation] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"[INFO] Federate: " << federateName << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - joinFederation] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateName << L" waiting for sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[INFO] Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - waitForSyncPoint] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::initializeHandles() {
    try {
        // For setup object class ship and its attributes. Subscribe
        std::wcout << L"[INFO] Initializing handles for ship" << std::endl;
        federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));

        // For setup object class missile and its attributes. Publish
        std::wcout << L"[INFO] Initializing handles for Missile" << std::endl;
        federateAmbassador->setObjectClassHandleMissile(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Missile"));
        federateAmbassador->setAttributeHandleMissileID(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileID"));
        federateAmbassador->setAttributeHandleMissileTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileTeam"));
        federateAmbassador->setAttributeHandleMissilePosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Position"));
        federateAmbassador->setAttributeHandleMissileAltitude(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Altitude"));
        federateAmbassador->setAttributeHandleMissileSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Speed"));

        // For setup object class ship and its attributes. Subscribe
        std::wcout << L"[INFO] Initializing handles for ship" << std::endl;
        federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));

        // For setup interaction class TargetHit and its parameters. Subscribe
        federateAmbassador->setInteractionClassTargetHit(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.TargetHit"));
        federateAmbassador->setParamTargetHitID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetID"));
        federateAmbassador->setParamTargetHitTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetTeam"));
        federateAmbassador->setParamTargetHitPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetPosition"));
        federateAmbassador->setParamTargetHitDestroyed(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetDestroyed"));
        
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - initializeHandles] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleShipSize());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfMissiles());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - subscribeAttributes] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::publishAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleMissileID());
        attributes.insert(federateAmbassador->getAttributeHandleMissileTeam());
        attributes.insert(federateAmbassador->getAttributeHandleMissilePosition());
        attributes.insert(federateAmbassador->getAttributeHandleMissileAltitude());
        attributes.insert(federateAmbassador->getAttributeHandleMissileSpeed());
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getObjectClassHandleMissile(), attributes);
        std::wcout << L"Published missile attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - publishAttributes] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::subscribeInteractions() {
    try {
        // Subscribe to interaction classes
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassSetupSimulation());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassSetupSimulation()) 
                   << std::endl;

        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassFireMissile()) 
                   << std::endl;


    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - subscribeInteractions] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassTargetHit());
        std::wcout << L"Published interaction: " 
                    << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassTargetHit()) 
                    << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - publishInteractions] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::waitForSetupSync() {

}

void MissileFederate::initializeTimeFactory() {
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

void MissileFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
    try {
        if (federateAmbassador->getIsRegulating()) {  // Prevent enabling twice
            std::wcout << L"[WARNING] Time Regulation already enabled. Skipping..." << std::endl;
            return;
        }
        /*
        Lookahead is the minimum amount of time the federate can look into the future
        and makes sure that the logical time must advance by at least this amount before 
        it can send an event or update attributes.
        */
       
        lookAhead = rti1516e::HLAfloat64Interval(0.5);  // Lookahead must be > 0
        std::wcout << L"[INFO] Enabling Time Management..." << std::endl;
        
        rtiAmbassador->enableTimeRegulation(lookAhead);
        while (!federateAmbassador->getIsRegulating()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS] Time Regulation enabled." << std::endl;

        rtiAmbassador->enableTimeConstrained();
        while (!federateAmbassador->getIsConstrained()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS] Time Constrained enabled." << std::endl;

    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"[ERROR] Exception during enableTimeManagement: " << e.what() << std::endl;
    }
}

void MissileFederate::setupMissileVisualization() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "[ERROR - setupMissileVisualization] Socket creation failed." << std::endl;
        return;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(MISSILE_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "[DEBUG] Failed to connect to the visual representation program." << std::endl;
        close(client_socket);
        return;
    }
    std::wcout << L"[SUCCESS] Connected to the visual representation program." << std::endl;
}

void MissileFederate::readyCheck() {
    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"MissilesCreated", rti1516e::VariableLengthData());
        std::wcout << L"[INFO] Announced synchronization point: MissileCreated" << std::endl;

        std::wcout << L"[INFO] All federates are ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Exception during readyCheck: " << e.what() << std::endl;
        exit(1);
    }
}

void MissileFederate::runSimulationLoop() {
    double stepsize = 0.5;
    double simulationTime = federateAmbassador->getCurrentLogicalTime();
    missile.initialDistanceToTarget = calculateDistance(
        missile.position,
        missile.initialTargetPosition,
        0.0
    );
    missile.id = missile.id + L"_" + std::to_wstring(getpid());
    missile.initialTargetID = missile.targetID;
    federateAmbassador->setMissile(missile);
    missile.startTime = std::chrono::high_resolution_clock::now();

    // Ensure logical time is initialized correctly
    if (!logicalTimeFactory) {
        std::wcerr << L"[ERROR] Logical time factory is null" << std::endl;
        exit(1);
    }

    std::wcout << L"[DEBUG] Starting simulation loop" << std::endl;

    while (federateAmbassador->getSyncLabel() != L"ReadyToExit") {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        //Calculations
        missile.speed = speedDis(gen);
        missile.groundDistanceToTarget = calculateDistance(
            missile.position,
            missile.initialTargetPosition,
            0.0
        );
        missile.distanceToTarget = calculateDistance(
            missile.position,
            missile.initialTargetPosition,
            missile.altitude
        );

        if (!missile.heightAchieved) {
            missile.altitude = increaseAltitude(
                missile.altitude, 
                missile.speed, 
                missile.distanceToTarget
            );
            if (missile.groundDistanceToTarget <= missile.initialDistanceToTarget/2) {
                missile.heightAchieved = true;
            }
        }
        else {
            if (missile.targetFound) {
                missile.altitude = reduceAltitude(
                    missile.altitude, 
                    missile.speed, 
                    missile.distanceToTarget
                );
                calculateYBearing(
                    missile.altitude,
                    missile.groundDistanceToTarget, 
                    0.0
                );
            } 
        }
        missile.position = calculateNewPosition(
            missile.position,
            missile.speed,
            missile.bearing
        );

        if (missile.groundDistanceToTarget < missile.distanceToTarget && !missile.targetFound) {
            missile.lookingForTarget = true;
        }
        

        //Info output
        std::wcout << L"[INFO] Simulation time: " << logicalTime << std::endl;
        std::wcout << L"[INFO] FederateName: " << federateName << std::endl;
        std::wcout << L"Missile ID: " << missile.id << std::endl 
        << L"Missile Team: " << missile.team << std::endl
        << L"Missile Position: " << missile.position.first << L", " << missile.position.second << std::endl
        << L"Missile Target Position: " << missile.initialTargetPosition.first << L", " << missile.initialTargetPosition.second << std::endl
        << L"Missile Altitude: " << missile.altitude << std::endl
        << L"Missile Speed: " << missile.speed << std::endl
        << L"Missile Bearing: " << missile.bearing << std::endl
        << L"Missile Distance to Target: " << missile.distanceToTarget << std::endl
        << L"Missile Ground Distance to Target: " << missile.groundDistanceToTarget << std::endl
        << L"Missile Target Found: " << missile.targetFound << std::endl;

        if (missile.distanceToTarget < 100) { //Add target locked condition here
            missile.targetDestroyed = true;
            sendTargetHitInteraction(missile, logicalTime);
            std::wcout << L"[INFO] Target destroyed." << std::endl;

            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> realtimeDuration = endTime - missile.startTime;
            double realtime = realtimeDuration.count();

            const auto& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(logicalTime);
            double federateSimulationTime = floatTime.getTime();

            std::wstringstream finalData; 
            finalData << L"--------------------------------------------------------\n"
                << L"Missile ID: " << missile.id << L"\n"
                << L"Missile Team: " << missile.team << L"\n"
                << L"Initial Target ID: " << missile.initialTargetID << L"\n"
                << L"Final Target ID: " << missile.targetID << L"\n"
                << L"Original distance to target: " << missile.initialDistanceToTarget << L"\n"
                << L"Missile Position: " << missile.position.first << L", " << missile.position.second << L"\n"
                << L"Missile Target Position: " << missile.initialTargetPosition.first << L", " << missile.initialTargetPosition.second << L"\n"
                << L"Simulation Time: " << federateSimulationTime << L"\n"
                << L"Flight Time (RealTime): " << std::to_wstring(realtime) << L" seconds";

                std::wofstream outFile(DATA_LOG_PATH, std::ios::app);
                if (outFile.is_open()) {
                    outFile << finalData.str() << std::endl;
                    outFile.close();
                } else {
                    std::wcerr << L"[ERROR] Failed to open log file." << std::endl;
                }


            resignFederation();
        }

        federateAmbassador->setIsAdvancing(true);
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
   
        simulationTime += stepsize;
    }
}

void MissileFederate::sendTargetHitInteraction(Missile& missile, const rti1516e::LogicalTime& logicalTime) {
    try {
        std::wcout << L"[SUCCESS] Target HIT, interaction sent at time " << logicalTime << std::endl;

        rti1516e::ParameterHandleValueMap parameters;
        parameters[federateAmbassador->getParamTargetHitID()] = rti1516e::HLAunicodeString(missile.targetID).encode();
        parameters[federateAmbassador->getParamTargetHitTeam()] = rti1516e::HLAunicodeString(missile.team).encode();

        rtiAmbassador->sendInteraction(
            federateAmbassador->getInteractionClassTargetHit(), 
            parameters, 
            rti1516e::VariableLengthData(),
            logicalTime);

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR - sendTargetHitInteraction] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR - resignFederation] Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::setMissile(const Missile& tmpMissile) {
    missile = tmpMissile;
}

int main(int argc, char* argv[]) {

    //Clear current log file
    if (argc < 7) {
        std::wcerr << L"[ERROR - main] Not enough arguments provided." << std::endl;
        return 1;
    }

    try {
        std::wcout << std::endl << L"[INFO - main] Starting MissileFederate..." << std::endl << std::endl;

        MissileFederate missileManagerFederate;
        Missile missile;
        
        missile.id = stringToWString(argv[1]);
        missile.targetID = stringToWString(argv[2]);
        missile.team = stringToWString(argv[3]);
        missile.position = stringToPair(argv[4]);
        missile.initialTargetPosition = stringToPair(argv[5]);
        missile.bearing = std::stod(argv[6]);

        missileManagerFederate.setMissile(missile);
        missileManagerFederate.startMissileManager(); // Call the member function
    } catch (const std::exception& e) {
        std::wcerr << L"[ERROR - main] Exception: " << e.what() << std::endl;
    }

    return 0;
}