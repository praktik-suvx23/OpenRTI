#include "ShipFederate.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(10.0, 25.0);

ShipFederate::ShipFederate() {
    createRTIAmbassador();
}


ShipFederate::~ShipFederate() {
    resignFederation();
}

void ShipFederate::startShip(int team) {
    std::wcout << L"ShipFederate starting" << std::endl;
    if (team == 1) {
        federateName = L"BlueShipFederate_" + std::to_wstring(getpid());
    } else {
        federateName = L"RedShipFederate_" + std::to_wstring(getpid());
    }
    federateAmbassador->setFederateName(federateName);
    try {
        connectToRTI();
        initializeFederation();
        joinFederation();
        waitForSyncPoint();
        initializeHandles();
        publishAttributes();
        subscribeAttributes();
        subscribeInteractions();
        publishInteractions();
        waitForSetupSync();
        createShipsSyncPoint();
        initializeTimeFactory();
        enableTimeManagement();
        readyCheck();
        runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] start Ship - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::createRTIAmbassador() {
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        federateAmbassador = std::make_unique<MyShipFederateAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIAmbassador - Exception" << e.what() << std::endl;
    }
}

void ShipFederate::connectToRTI() {
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

void ShipFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Federate: " << federateName << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateName << L" waiting for sync point";
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L" || Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::initializeHandles() {

    //Object Handles for publishing 
    federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
    federateAmbassador->setAttributeHandleShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
    federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
    federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
    federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
    federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));
    std::wcout << L"ObjectClassHandleShip: " << federateAmbassador->getObjectClassHandleShip() << " initialized" << std::endl;

    //Interaction class handles for SetupInteraction
    federateAmbassador->setSetupSimulationHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
    federateAmbassador->setBlueShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfBlueShips"));
    federateAmbassador->setRedShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfRedShips"));
    federateAmbassador->setTimeScaleFactorParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"TimeScaleFactor"));
    std::wcout << L"SetupSimulationClassHandle: " << federateAmbassador->getSetupSimulationHandle() << " initialized" << std::endl;

    federateAmbassador->setInteractionClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
    federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterID"));
    federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"Team"));
    federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterPosition"));
    federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetPosition"));
    federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"NumberOfMissilesFired"));    
    std::wcout << L"InteractionClassFireMissile: " << federateAmbassador->getInteractionClassFireMissile() << " initialized" << std::endl;

    federateAmbassador->setInteractionClassTargetHit(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.TargetHit"));
    federateAmbassador->setParamTargetHitID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetID"));
    federateAmbassador->setParamTargetHitTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetTeam"));
    federateAmbassador->setParamTargetHitPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetPosition"));
    federateAmbassador->setParamTargetHitDestroyed(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetDestroyed"));
    std::wcout << L"InteractionClassTargetHit: " << federateAmbassador->getInteractionClassTargetHit() << " initialized" << std::endl;
}

void ShipFederate::publishAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfMissiles());
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"Published ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfMissiles());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        std::wcout << L"Published interaction class: FireMissile" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getSetupSimulationHandle());
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassTargetHit());
        std::wcout << L"Subscribed to SetupSimulation interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::waitForSetupSync() {
    std::wcout << L"[DEBUG] federate: " << federateName << L" waiting for setup sync point";
    try {
        while (federateAmbassador->getSyncLabel() != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"|| Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::createShipsSyncPoint() {
    if (federateName.find(L"BlueShipFederate") != 0 && federateName.find(L"RedShipFederate") != 0) {
        std::wcerr << L"[ERROR] " << federateName << L" - not a valid ship federate." << std::endl;
        resignFederation();
        exit(1);
    }

    try {
        while (!federateAmbassador->getCreateShips()) {
            std::wcout << L"Waiting for ships to be created" << std::endl;
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[INFO] " << federateName << L" - have created it's ships. " << std::endl;

        rtiAmbassador->registerFederationSynchronizationPoint(federateName, rti1516e::VariableLengthData());

        if (federateName.find(L"BlueShipFederate") == 0) {
            while (federateAmbassador->getBlueSyncLabel() != L"BlueShipFederate") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
        if (federateName.find(L"RedShipFederate") == 0) {
            while (federateAmbassador->getRedSyncLabel() != L"RedShipFederate") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
        std::wcout << L"[INFO] " << federateName << L" - registered sync point: " << federateName;

        while (federateAmbassador->getRedSyncLabel() != L"RedShipFederate" || federateAmbassador->getBlueSyncLabel() != L"BlueShipFederate") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L" || " << federateName << L" - received sync point." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createShipsSyncPoint - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::initializeTimeFactory() {
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

void ShipFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
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
        auto lookahead = rti1516e::HLAfloat64Interval(0.5);  // Lookahead must be > 0
        std::wcout << L"[INFO] Enabling Time Management...";
        
        rtiAmbassador->enableTimeRegulation(lookahead);
        while (!federateAmbassador->getIsRegulating()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS]" << std::endl;

        std::wcout << L"[INFO] Enabling Time Constrained...";
        rtiAmbassador->enableTimeConstrained();
        while (!federateAmbassador->getIsConstrained()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"[SUCCESS]" << std::endl;

    } catch (const rti1516e::Exception &e) {
        std::wcerr << L"[ERROR] Exception during enableTimeManagement: " << e.what() << std::endl;
    }
}

void ShipFederate::readyCheck() {
    try {
        while (federateAmbassador->getSyncLabel() != L"MissileReady") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        if (federateName.find(L"BlueShipFederate") == 0) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"BlueShipReady", rti1516e::VariableLengthData());

            while (federateAmbassador->getSyncLabel() != L"BlueShipReady") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
        if (federateName.find(L"RedShipFederate") == 0) {
            while (federateAmbassador->getSyncLabel() != L"BlueShipReady") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }

            rtiAmbassador->registerFederationSynchronizationPoint(L"RedShipReady", rti1516e::VariableLengthData());
            while (federateAmbassador->getSyncLabel() != L"RedShipReady") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
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

void ShipFederate::runSimulationLoop() {
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double maxTargetDistance = 80000.0; //Change when needed

    std::wcout << L"[SIM] Running simulation loop..." << std::endl;
    do {
        if (!logicalTimeFactory) {
            std::wcerr << L"[ERROR] Logical time factory is null." << std::endl;
            exit(1);
        }
    
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
    
        // === Update Ship Attributes ===
        std::wcout << L"[INFO] Ship - Updating own attributes..." << std::endl;
        for (Ship* ship : federateAmbassador->getOwnShips()) {
            rti1516e::AttributeHandleValueMap attributes;
    
            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship->shipPosition.second));
    
            attributes[federateAmbassador->getAttributeHandleShipFederateName()] = rti1516e::HLAunicodeString(ship->shipName).encode();
            attributes[federateAmbassador->getAttributeHandleShipTeam()] = rti1516e::HLAunicodeString(ship->shipTeam).encode();
            attributes[federateAmbassador->getAttributeHandleShipSpeed()] = rti1516e::HLAfloat64BE(ship->shipSpeed).encode();
            attributes[federateAmbassador->getAttributeHandleShipPosition()] = shipPositionRecord.encode();
            attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(ship->shipNumberOfMissiles).encode();
    
            rtiAmbassador->updateAttributeValues(ship->objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);
        }

        // === Enemy Detection & Targeting ===
        if (federateAmbassador->getFederateName().find(L"BlueShipFederate") == 0) {
            std::wcout << L"[INFO] BlueShipFederate - Detecting enemies..." << std::endl;
            for (Ship* blueShip : federateAmbassador->getOwnShips()) {
                Ship* closestEnemy = nullptr;
                double closestDistance = std::numeric_limits<double>::max();
                for (Ship* redShip : federateAmbassador->getRedShips()) {
                    double distance = calculateDistance(blueShip->shipPosition, redShip->shipPosition, 0);
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestEnemy = redShip;
                    }
        
                    if (distance < maxTargetDistance && redShip->currentMissilesLocking < redShip->maxMissilesLocking) {
                        federateAmbassador->closestMissileRangeToTarget.insert({ distance, { blueShip, redShip } });
                    }
                }
                if (closestEnemy != nullptr) {
                    federateAmbassador->setClosestEnemyShip(blueShip, closestEnemy);
                }
            }
        } else if (federateAmbassador->getFederateName().find(L"RedShipFederate") == 0) {
            std::wcout << L"[INFO] RedShipFederate - Detecting enemies..." << std::endl;
            for (Ship* redShip : federateAmbassador->getOwnShips()) {
                Ship* closestEnemy = nullptr;
                double closestDistance = std::numeric_limits<double>::max();
    
                for (Ship* blueShip : federateAmbassador->getBlueShips()) {
                    double distance = calculateDistance(redShip->shipPosition, blueShip->shipPosition, 0);
        
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestEnemy = blueShip;
                    }
        
                    if (distance < maxTargetDistance && blueShip->currentMissilesLocking < blueShip->maxMissilesLocking) {
                        federateAmbassador->closestMissileRangeToTarget.insert({ distance, { redShip, blueShip } });
                    }
                }
                if (closestEnemy != nullptr) {
                    federateAmbassador->setClosestEnemyShip(redShip, closestEnemy);
                }
            }
        }
        
    
        // === Fire Missiles at Targets ===
        for (const auto& [distance, pair] : federateAmbassador->closestMissileRangeToTarget) {
            Ship* ship = pair.first;
            Ship* enemyShip = pair.second;
            if (ship->shipTeam == enemyShip->shipTeam) {
                std::wcout << L"[INFO] Skipping friendly fire from " << ship->shipName << L" to " << enemyShip->shipName << std::endl;
                continue;
            }
            while (ship->shipNumberOfMissiles > 0 && enemyShip->currentMissilesLocking < enemyShip->maxMissilesLocking) {
                // Calculate the maximum number of missiles to fire in this interaction
                int maxMissilesToFire = std::min(
                    static_cast<int>(ship->shipNumberOfMissiles),
                    static_cast<int>(enemyShip->maxMissilesLocking - enemyShip->currentMissilesLocking)
                );
            
                if (maxMissilesToFire <= 0) {
                    break; // No missiles can be fired
                }
            
                // Log the interaction
                std::wcout << L"[FIRE] " << ship->shipName << L" fired " << maxMissilesToFire
                           << L" missile(s) at " << enemyShip->shipName << std::endl;
            
                // Update the state of the ships
                enemyShip->currentMissilesLocking += maxMissilesToFire;
                ship->shipNumberOfMissiles -= maxMissilesToFire;
            
                // Send the interaction
                prepareMissileLaunch(logicalTime, maxMissilesToFire, *ship, *enemyShip);
            }
        }
    
        // === Move Ships Based on Closest Enemy ===
        for (const auto& [ship, enemyShip] : federateAmbassador->getClosestEnemyShip()) {
            if (ship->shipTeam == enemyShip->shipTeam) {
                std::wcout << L"[INFO] Skipping friendly fire from " << ship->shipName << L" to " << enemyShip->shipName << std::endl;
                continue;
            }

            double bearing = calculateBearing(ship->shipPosition, enemyShip->shipPosition);
            ship->shipSpeed = getSpeed(10, 10, 25);
            ship->shipPosition = calculateNewPosition(ship->shipPosition, ship->shipSpeed, bearing);
        }

        for(auto i : federateAmbassador->getFireOrderMap()) {
            Ship* ship = i.first;
            Ship* targetShip = i.second.first;
            int fireAmount = i.second.second;

            sendInteraction(logicalTime, fireAmount, *ship, *targetShip);
        }
    
        federateAmbassador->closestMissileRangeToTarget.clear();
        federateAmbassador->clearClosestEnemyShip();
        federateAmbassador->clearFireOrderMap();
        federateAmbassador->setIsAdvancing(true);
    
        rtiAmbassador->timeAdvanceRequest(logicalTime);
        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    
        simulationTime += stepsize;
        
        // === Exit conditions ===
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - federateAmbassador->startTime).count();
        if (federateAmbassador->getBlueShips().empty() && federateAmbassador->getSyncLabel() != L"ReadyToExit" && elapsedTime > 10) {
            waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
        if ((federateAmbassador->getSyncLabel() == L"RedShipEmpty" && federateName.find(L"BlueShipFederate") == 0) ||
            (federateAmbassador->getSyncLabel() == L"BlueShipEmpty" && federateName.find(L"RedShipFederate") == 0)) {
            waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
    } while (federateAmbassador->getSyncLabel() != L"ReadyToExit");    
}

void  ShipFederate::prepareMissileLaunch(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const Ship& targetShip) {
    rti1516e::ParameterHandleValueMap parameters;
    try {
        parameters[federateAmbassador->getParamNumberOfMissilesFired()] = rti1516e::HLAinteger32BE(fireAmount).encode();
        parameters[federateAmbassador->getParamShooterID()] = rti1516e::HLAunicodeString(ship.shipName).encode();
        parameters[federateAmbassador->getParamMissileTeam()] = rti1516e::HLAunicodeString(ship.shipTeam).encode();
        parameters[federateAmbassador->getParamTargetID()] = rti1516e::HLAunicodeString(targetShip.shipName).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch - Exception: " << e.what() << std::endl;
    }

    try {
        rtiAmbassador->sendInteraction(
            federateAmbassador->getInteractionClassFireMissile(),
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent FireMissile interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const Ship& targetShip) {
    rti1516e::ParameterHandleValueMap parameters;
    try {
        rti1516e::HLAfixedRecord shooterPositionRecord;
        shooterPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship.shipPosition.first));
        shooterPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship.shipPosition.second));
    
        rti1516e::HLAfixedRecord targetPositionRecord;
        targetPositionRecord.appendElement(rti1516e::HLAfloat64BE(targetShip.shipPosition.first));
        targetPositionRecord.appendElement(rti1516e::HLAfloat64BE(targetShip.shipPosition.second));
       
        parameters[federateAmbassador->getParamShooterID()] = rti1516e::HLAunicodeString(ship.shipName).encode();
        parameters[federateAmbassador->getParamMissileTeam()] = rti1516e::HLAunicodeString(ship.shipTeam).encode();
        parameters[federateAmbassador->getParamMissileStartPosition()] = shooterPositionRecord.encode();
        parameters[federateAmbassador->getParamMissileTargetPosition()] = targetPositionRecord.encode();
        parameters[federateAmbassador->getParamNumberOfMissilesFired()] = rti1516e::HLAinteger32BE(fireAmount).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] sendInteraction - Exception: " << e.what() << std::endl;
    }
  
    try {
        rtiAmbassador->sendInteraction(
            federateAmbassador->getInteractionClassFireMissile(),
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent FireMissile interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEUG] sendInteraction - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::waitForExitLoop(double simulationTime, double stepsize) {
    int pulse = 0;
    std::wcout << L"[INFO] Waiting in exit loop. Simulation time: " << simulationTime << std::endl;
    try {
        //This wont work with several Ship instances
        if (federateName.find(L"BlueShipFederate") == 0) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"BlueShipEmpty", rti1516e::VariableLengthData());
            while (federateAmbassador->getSyncLabel() != L"BlueShipEmpty") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        } 
        else if (federateName.find(L"RedShipFederate") == 0) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"RedShipEmpty", rti1516e::VariableLengthData());
            while (federateAmbassador->getSyncLabel() != L"RedShipEmpty") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] waitForExitLoop - Exception: " << e.what() << std::endl;
    }

    while (federateAmbassador->getSyncLabel() != L"ReadyToExit") {
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        federateAmbassador->setIsAdvancing(true);
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - federateAmbassador->startTime).count();
    
        if (elapsedTime / 10 > pulse) {
            pulse = elapsedTime / 10;
            std::wcout << L"[EXIT-PULSE] Elapsed time: " << elapsedTime << L" seconds." << std::endl;
        }

        simulationTime += stepsize;
    }
    std::wcout << L"[INFO] Exiting simulation loop. Simulation time: " << simulationTime << std::endl;
}

void ShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int team;
    while (true) {
        std::wcout << L"Pick blue team (1) or red team (2)" << std::endl;
        std::wcin >> team;
        if (team != 1 && team != 2) {
            std::wcerr << L"Invalid team selection" << std::endl;
            return 1;
        }
        else {
            break;
        }
    }

    ShipFederate ShipFederate;
    ShipFederate.startShip(team);

    return 0;
}