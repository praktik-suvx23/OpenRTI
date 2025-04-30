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
    initializeFederation();
    joinFederation();
    waitForSyncPoint();
    initializeHandles();
    subscribeAttributes();
    publishAttributes();
    subscribeInteractions();
    publishInteractions();
    setupMissileVisualization();
    waitForSetupSync();
    initializeTimeFactory();
    enableTimeManagement();
    std::wcout << L"[DEBUG] Starting \"readyCheck\"..." << std::endl;
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


        // For setup interaction class StartSimulation and its parameters. Subscribe
        std::wcout << L"[INFO] Initializing handles for setupSimulation" << std::endl;
        federateAmbassador->setInteractionClassSetupSimulation(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
        federateAmbassador->setParamTimeScaleFactor(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassSetupSimulation(), L"TimeScaleFactor"));

        // For setup interaction class FireMissile and its parameters. Subscribe
        std::wcout << L"[INFO] Initializing handles for fire missile" << std::endl;
        federateAmbassador->setInteractionClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
        federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterID"));
        federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"Team"));
        federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterPosition"));
        federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetPosition"));
        federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"NumberOfMissilesFired"));
        
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
    std::wcout << L"[INFO] federate: " << federateName << L" waiting for setup sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] waitForSetupSync - \"SimulationSetupComplete\" - Exception: " << e.what() << std::endl;
    }

    std::wcout << L"[INFO] Waiting for all ships to be created..." << std::endl;
    try {
        while (federateAmbassador->getRedSyncLabel() != L"RedShipFederate" || federateAmbassador->getBlueSyncLabel() != L"BlueShipFederate") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] waitForSetupSync - \"RedShipFederate||BlueShipFederate\" - Exception: " << e.what() << std::endl;
    }
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
        while (federateAmbassador->getSyncLabel() != L"AdminReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        rtiAmbassador->registerFederationSynchronizationPoint(L"MissileReady", rti1516e::VariableLengthData());
        std::wcout << L"[INFO] Announced synchronization point: MissileReady" << std::endl;

        while (federateAmbassador->getSyncLabel() != L"MissileReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        while (federateAmbassador->getSyncLabel() != L"EveryoneReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"[INFO] All federates are ready." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR] Exception during readyCheck: " << e.what() << std::endl;
        exit(1);
    }
}

void MissileFederate::runSimulationLoop() {
    bool heightAchieved = false;
    double stepsize = 0.5;
    double simulationTime = 0.0;

    // Ensure logical time is initialized correctly
    if (!logicalTimeFactory) {
        std::wcerr << L"[ERROR] Logical time factory is null" << std::endl;
        exit(1);
    }

    std::wcout << L"[DEBUG] Starting simulation loop" << std::endl;

    while (federateAmbassador->getSyncLabel() != L"ReadyToExit") {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        if (federateAmbassador->getMissiles().empty()) {

            federateAmbassador->setIsAdvancing(true);
            rtiAmbassador->timeAdvanceRequest(logicalTime);

            while (federateAmbassador->getIsAdvancing()) {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }

            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            simulationTime += stepsize;
            continue;
        }

        for (auto i = federateAmbassador->getMissiles().begin(); i != federateAmbassador->getMissiles().end();) 
        {
            Missile& missile = *i;
            if (missile.targetDestroyed) {
                missile.speed = 0.0;
                continue;
            }
            else {
                missile.speed = getSpeed(missile.speed, 250.0, 450.0);
                missile.groundDistanceToTarget = calculateDistance(
                    missile.position, 
                    missile.initialTargetPosition, 
                    0.0);
            }
            missile.distanceToTarget = calculateDistance(
                missile.position, 
                missile.initialTargetPosition, 
                missile.altitude);

            if (!missile.heightAchieved) {
                //std::wcout << L"[INFO] Increasing altitude" << std::endl;
                missile.altitude = increaseAltitude(
                    missile.altitude, 
                    missile.speed, 
                    missile.distanceToTarget);
                if (missile.groundDistanceToTarget <= missile.initialDistanceToTarget/2) {
                    missile.heightAchieved = true;
                }
            } else {
                if (missile.targetFound) {
                    missile.altitude = reduceAltitudeV2(
                        missile.altitude, 
                        missile.speed, 
                        missile.distanceToTarget,
                        calculateYBearing(
                            missile.altitude, 
                            missile.groundDistanceToTarget, 
                            0.0));
                }
            }
            
            
            missile.position = calculateNewPosition(
                missile.position, 
                missile.speed, 
                missile.bearing);
            //recude number of missiles targeting

            if (client_socket > 0) {
                send_missile(client_socket, missile);
            }

            std::wcout << L"-------------------------------------------------------" << std::endl;
            std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile ID: " << missile.id << std::endl;
            std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile Team: " << missile.team << std::endl;
            std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile Altitude: " << missile.altitude << " meters" << std::endl;
            std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile Speed: " << missile.speed << " m/s" << std::endl;
            std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile Current Position: " << missile.position.first << ", " << missile.position.second << std::endl;
            if (missile.targetFound) {
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Target Ship ID: " << missile.targetID << std::endl;
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Target Ship Position: " << missile.initialTargetPosition.first << ", " << missile.initialTargetPosition.second << std::endl;
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Current direction for missile " << missile.bearing << std::endl;
            }
            else 
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Initial Target Position: " << missile.initialTargetPosition.first << ", " << missile.initialTargetPosition.second << std::endl;
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Distance between missile and target: " << missile.distanceToTarget << " meters" << std::endl;
            if (missile.groundDistanceToTarget < missile.distanceToTarget && !missile.targetFound) {
                missile.lookingForTarget = true;
                std::wcout << L"[INFO - " << missile.objectInstanceHandle.hash() << "] Missile searching for target" << std::endl;
            }
            std::wcout << L"#######################################################" << std::endl;
            //Check if target object still exists

            if (missile.distanceToTarget < 300) {
                
                sendTargetHitInteraction(missile, logicalTime); 

                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> realTimeDuration = endTime - missile.startTime;
                double realTime = realTimeDuration.count();
                const auto& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(logicalTime);
                double federateSimulationTime = floatTime.getTime();

                std::vector<std::wstring> finalData = {
                    L"--------------------------------------------",
                    L"Instance : " + std::to_wstring(missile.objectInstanceHandle.hash()),
                    L"Missile ID : " + missile.id + L" (" + missile.team + L")",
                    L"Target ID : " + missile.targetID,
                    L"Last Distance : " + std::to_wstring(missile.distanceToTarget) + L" meters",
                    L"Last Altitude : " + std::to_wstring(missile.altitude) + L" meters",
                    L"Last Speed : " + std::to_wstring(missile.speed) + L" m/s",
                    L"Last position for missile : " + 
                        std::to_wstring(missile.position.first) + 
                        L", " + 
                        std::to_wstring(missile.position.second),
                    L"Last position for ship : " + 
                        std::to_wstring(missile.initialTargetPosition.first) + 
                        L", " + 
                        std::to_wstring(missile.initialTargetPosition.second),
                    L"Simulation time : " + std::to_wstring(federateSimulationTime) + L" seconds",
                    L"Real time : " + std::to_wstring(realTime) + L" seconds",
                    L"--------------------------------------------"
                };

                std::ofstream outFile(DATA_LOG_PATH, std::ios::app);
                if (outFile.is_open()) {
                    for (const auto& entry : finalData) {
                        outFile << std::string(entry.begin(), entry.end()) << std::endl;
                    }
                    outFile.close();
                    std::wcout << L"[SUCCESS] Data successfully written to finalData.txt" << std::endl;
                } else {
                    std::wcerr << L"[DEBUG] Unable to open file: finalData.txt" << std::endl;
                }

                std::wcout << L"[INFO] " << missile.id << " Target reached" << std::endl;
                std::wcout << L"[INFO] Distance before last contact: " << missile.distanceToTarget << " meters" << std::endl;

                if (federateAmbassador->removeMissileObject(missile.objectInstanceHandle)) 
                    std::wcout << L"[INFO] Missile removed: " << missile.objectInstanceHandle.hash() << std::endl;
                
            }
            if (missile.targetDestroyed) {
                std::wcout << L"[INFO] Target destroyed" << std::endl;
            } else {
                ++i;
            }
        }
        
        federateAmbassador->setIsAdvancing(true);
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        simulationTime += stepsize;
    }
    
    //Debug output of missile targets -----------------------------------------------------
    //-----------------------------------------------------------------------------------
    const auto& debugOutput = federateAmbassador->MissileTargetDebugOutPut;

    int totalMissiles = debugOutput.size();
    int emptyTargets = 0;

    std::wofstream outFile(DATA_LOG_PATH, std::ios::app); // Append to the log file
    if (!outFile.is_open()) {
        std::wcerr << L"[ERROR] Could not open finalData.txt for writing." << std::endl;
        return;
    }

    outFile << L"--------------------------------------------" << std::endl;
    outFile << L"Final Missile Target Summary:" << std::endl;

    for (const auto& entry : debugOutput) {
        outFile << entry << std::endl;

        // Check if the Target ID is empty
        size_t targetIDPos = entry.find(L"Target ID :");
        if (targetIDPos != std::wstring::npos) {
            size_t targetValueStart = targetIDPos + std::wstring(L"Target ID :").length();
            std::wstring targetValue = entry.substr(targetValueStart);
            if (targetValue.find_first_not_of(L" \t\n\r") == std::wstring::npos) {
                emptyTargets++;
            }
        }
    }

    double emptyTargetPercentage = (static_cast<double>(emptyTargets) / totalMissiles) * 100.0;

    outFile << L"--------------------------------------------" << std::endl;
    outFile << L"Total Missiles: " << totalMissiles << std::endl;
    outFile << L"Missiles with Empty Targets: " << emptyTargets << std::endl;
    outFile << L"Percentage of Empty Targets: " << emptyTargetPercentage << L"%" << std::endl;
    outFile << L"--------------------------------------------" << std::endl;

    outFile.close();

    std::wcout << L"[INFO] Final output written to finalData.txt" << std::endl;

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

int main(int argc, char* argv[]) {

    std::wofstream outFile(DATA_LOG_PATH, std::ios::trunc); //See Data_LOG_PATH in CMakeLists.txt

    try {
        MissileFederate missileManagerFederate;
        missileManagerFederate.startMissileManager(); // Call the member function
    } catch (const std::exception& e) {
        std::wcerr << L"[ERROR - main] Exception: " << e.what() << std::endl;
    }

    return 0;
}