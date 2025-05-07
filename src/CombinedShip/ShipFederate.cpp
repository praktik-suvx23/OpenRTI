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
        federateAmbassador->setTeamStatus(ShipTeam::BLUE);
    } else {
        federateName = L"RedShipFederate_" + std::to_wstring(getpid());
        federateAmbassador->setTeamStatus(ShipTeam::RED);
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
    federateAmbassador->setAttributeHandleMaxMissilesLockingThisShip(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"MaxMissileLockingThisShip"));
    federateAmbassador->setAttributeHandleCurrentMissilesLockingThisShip(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"CurrentAmountOfMissilesLockingThisShip"));
    std::wcout << L"ObjectClassHandleShip: " << federateAmbassador->getObjectClassHandleShip() << " initialized" << std::endl;

    //Interaction class handles for SetupInteraction
    federateAmbassador->setSetupSimulationHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
    federateAmbassador->setBlueShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfBlueShips"));
    federateAmbassador->setRedShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfRedShips"));
    federateAmbassador->setTimeScaleFactorParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"TimeScaleFactor"));
    std::wcout << L"SetupSimulationClassHandle: " << federateAmbassador->getSetupSimulationHandle() << " initialized" << std::endl;

    //Interaction class handles for FireMissile
    federateAmbassador->setInteractionClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
    federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterID"));
    federateAmbassador->setParamTargetID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetID"));
    federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"Team"));
    federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterPosition"));
    federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetPosition"));
    federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"NumberOfMissilesFired"));    
    std::wcout << L"InteractionClassFireMissile: " << federateAmbassador->getInteractionClassFireMissile() << " initialized" << std::endl;
    
    
    if (federateAmbassador->getTeamStatus() == ShipTeam::BLUE) {
        federateAmbassador->setInteractionClassConfirmRedHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.ConfirmHandshakeBlue"));
        federateAmbassador->setInteractionClassInitiateRedHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeBlue"));
        tempConfirmHandshake = federateAmbassador->getInteractionClassConfirmRedHandshake();
        tempInitiateHandshake = federateAmbassador->getInteractionClassInitiateRedHandshake();
    } else if (federateAmbassador->getTeamStatus() == ShipTeam::RED) {
        federateAmbassador->setInteractionClassConfirmBlueHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.ConfirmHandshakeRed"));
        federateAmbassador->setInteractionClassInitiateBlueHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeRed"));
        tempConfirmHandshake = federateAmbassador->getInteractionClassConfirmBlueHandshake();
        tempInitiateHandshake = federateAmbassador->getInteractionClassInitiateBlueHandshake();
    } else {
        std::wcerr << L"[DEBUG] Error: Team status not set correctly" << std::endl;
        resignFederation();
    }
    //Interaction class handles for InitiateHandshake
    federateAmbassador->setParamInitiateShooterID(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"ShooterShipID"));
    federateAmbassador->setParamInitiateShooterTeam(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"ShooterShipTeam"));
    federateAmbassador->setParamInitiateTargetID(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"TargetShipID"));
    federateAmbassador->setParamInitiateMissileAmountFired(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"MissileAmountFired"));
    federateAmbassador->setParamInitiateOrderID(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"OrderID"));
    std::wcout << L"InteractionClassInitiateHandshake: " << tempInitiateHandshake << " initialized" << std::endl;

    //Interaction class handles for ConfirmHandshake
    federateAmbassador->setParamConfirmShooterID(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"ShooterShipID"));
    federateAmbassador->setParamConfirmShooterTeam(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"ShooterShipTeam"));
    federateAmbassador->setParamConfirmTargetID(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"TargetShipID"));
    federateAmbassador->setParamConfirmMissileAmountFired(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"MissileAmountFired"));
    federateAmbassador->setParamConfirmOrderID(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"OrderID"));
    std::wcout << L"InteractionClassConfirmHandshake: " << tempConfirmHandshake << " initialized" << std::endl;

    //Interaction class handles for TargetHit
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
        attributes.insert(federateAmbassador->getAttributeHandleMaxMissilesLockingThisShip());
        attributes.insert(federateAmbassador->getAttributeHandleCurrentMissilesLockingThisShip());
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
        attributes.insert(federateAmbassador->getAttributeHandleMaxMissilesLockingThisShip());
        attributes.insert(federateAmbassador->getAttributeHandleCurrentMissilesLockingThisShip());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        rtiAmbassador->publishInteractionClass(tempInitiateHandshake);
        rtiAmbassador->publishInteractionClass(tempConfirmHandshake);
        std::wcout << L"Published interaction class: FireMissile" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        rtiAmbassador->subscribeInteractionClass(tempInitiateHandshake);
        rtiAmbassador->subscribeInteractionClass(tempConfirmHandshake);
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
    if (federateAmbassador->getTeamStatus() != ShipTeam::BLUE && federateAmbassador->getTeamStatus() != ShipTeam::RED) {
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

        if (federateAmbassador->getTeamStatus() == ShipTeam::BLUE) {
            while (federateAmbassador->getBlueSyncLabel() != L"BlueShipFederate") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
        if (federateAmbassador->getTeamStatus() == ShipTeam::RED) {
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

        if (federateAmbassador->getTeamStatus() == ShipTeam::BLUE) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"BlueShipReady", rti1516e::VariableLengthData());

            while (federateAmbassador->getSyncLabel() != L"BlueShipReady") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        }
        if (federateAmbassador->getTeamStatus() == ShipTeam::RED) {
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

    initializeLogFile(federateAmbassador->getTeamStatus());
    logToFile("[INFO] New run begun...", federateAmbassador->getTeamStatus());

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
            attributes[federateAmbassador->getAttributeHandleMaxMissilesLockingThisShip()] = rti1516e::HLAinteger32BE(ship->maxMissilesLocking).encode();
            attributes[federateAmbassador->getAttributeHandleCurrentMissilesLockingThisShip()] = rti1516e::HLAinteger32BE(ship->currentMissilesLocking).encode();
    
            rtiAmbassador->updateAttributeValues(ship->objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);
        }

        // === Enemy Detection & Targeting ===
        if (federateAmbassador->getAllowShipFire()) {
            if (!federateAmbassador->getClosestEnemyShip().empty()) {
                federateAmbassador->clearClosestEnemyShip();
            }
            detectEnemies(maxTargetDistance);
            std::vector<Ship*> friendlyShips = (federateAmbassador->getTeamStatus() == ShipTeam::BLUE) 
            ? federateAmbassador->getBlueShips() : federateAmbassador->getRedShips();
            if (!federateAmbassador->closestMissileRangeToTarget.empty() && !friendlyShips.empty()) {
                federateAmbassador->setAllowShipFire(false);
            }
        }
    
        // === Fire Missiles at Targets ===
        for (auto& [distance, pair] : federateAmbassador->closestMissileRangeToTarget) {
            Ship* ship = pair.first;
            Ship* enemyShip = pair.second;

            if (ship->shipTeam == enemyShip->shipTeam) continue;

            int alreadyLocked = tempLockingCount[enemyShip];
            int aviableLocking = enemyShip->maxMissilesLocking - enemyShip->currentMissilesLocking - alreadyLocked;
            if (aviableLocking <= 0) continue;

            int maxMissilesToFire = std::min(ship->shipNumberOfMissiles, aviableLocking);
            if (maxMissilesToFire <= 0) continue;

            if (enemyShip->currentMissilesLocking + tempLockingCount[enemyShip] > enemyShip->maxMissilesLocking) {
                std::wcout << L"[ERROR] " << enemyShip->shipName << L" already has "
                           << (enemyShip->currentMissilesLocking + tempLockingCount[enemyShip])
                           << L" missile(s) locking on it, which exceeds its maximum of "
                           << enemyShip->maxMissilesLocking << L"." << std::endl;
                continue;
            }

            logWmessage = L"[PREPARE MISSILE] " + ship->shipName + L" have " + std::to_wstring(ship->shipNumberOfMissiles)
                        + L" missile(s) and prepare to fire at:" + enemyShip->shipName + L" with " + std::to_wstring(maxMissilesToFire) + L"/" 
                        + std::to_wstring(enemyShip->maxMissilesLocking) + L" missile(s) locking on it.\n";
            logMessage = std::string(logWmessage.begin(), logWmessage.end());
            logToFile(logMessage, federateAmbassador->getTeamStatus());
        
            // If friendly federates aviable.
            if ((federateAmbassador->getTeamStatus() == ShipTeam::BLUE && federateAmbassador->getBlueShips().size() > 0) ||
                (federateAmbassador->getTeamStatus() == ShipTeam::RED && federateAmbassador->getRedShips().size() > 0)) {
                prepareMissileLaunch(logicalTime, maxMissilesToFire, *ship, *enemyShip, federateAmbassador->generateOrderID());
            } else {
                // If no friendly federates aviable.
                enemyShip->currentMissilesLocking += maxMissilesToFire;
                ship->shipNumberOfMissiles -= maxMissilesToFire;
                fireMissile(logicalTime, maxMissilesToFire, *ship, *enemyShip);
            }
            tempLockingCount[enemyShip] += maxMissilesToFire;
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

        for (const FireOrder& order : federateAmbassador->getInitialOrders()) {
            sendFireMissileInteraction(logicalTime, order.missileAmount, *order.shooterShip, *order.targetShip, order.orderID);
        }

        if (!federateAmbassador->getFireOrders().empty()) {
            std::wcout << L"[FIREORDER-DEBUG]" << std::endl;
        }

        for (const FireOrder& order : federateAmbassador->getFireOrders()) {
            logWmessage = L"[RECEIVED FIREORDER] " + order.shooterShip->shipName + L" have " + std::to_wstring(order.missileAmount)
                        + L" missile(s) and fire at:" + order.targetShip->shipName + L" with " + std::to_wstring(order.missileAmount) 
                        + L" missile(s) locking on it. OrderID: " + std::to_wstring(order.orderID) + L"\n";
            logMessage = std::string(logWmessage.begin(), logWmessage.end());
            logToFile(logMessage, federateAmbassador->getTeamStatus());
            fireMissile(logicalTime, order.missileAmount, *order.shooterShip, *order.targetShip);
        }
    
        federateAmbassador->closestMissileRangeToTarget.clear();
        federateAmbassador->clearInitialOrders();
        federateAmbassador->clearFireOrders();
        federateAmbassador->setIsAdvancing(true);
    
        rtiAmbassador->timeAdvanceRequest(logicalTime);
        while (federateAmbassador->getIsAdvancing()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    
        simulationTime += stepsize;
        
        // === Exit conditions ===
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - federateAmbassador->startTime).count();
        if (federateAmbassador->getOwnShips().empty() && federateAmbassador->getSyncLabel() != L"ReadyToExit" && elapsedTime > 10) {
            waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
        if ((federateAmbassador->getSyncLabel() == L"RedShipEmpty" && federateAmbassador->getTeamStatus() == ShipTeam::BLUE) ||
            (federateAmbassador->getSyncLabel() == L"BlueShipEmpty" && federateAmbassador->getTeamStatus() == ShipTeam::RED)) {
            waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
    } while (federateAmbassador->getSyncLabel() != L"ReadyToExit");    
}

void ShipFederate::prepareMissileLaunch(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const Ship& targetShip, int orderID) {
    rti1516e::ParameterHandleValueMap parameters;
    try {
        std::wcout << L"[DEBUG] Data being sent: " << ship.shipName << L" - " << ship.shipTeam << L" - " << targetShip.shipName << std::endl;
        parameters[federateAmbassador->getParamInitiateShooterID()] = rti1516e::HLAunicodeString(ship.shipName).encode();
        parameters[federateAmbassador->getParamInitiateShooterTeam()] = rti1516e::HLAunicodeString(ship.shipTeam).encode();
        parameters[federateAmbassador->getParamInitiateTargetID()] = rti1516e::HLAunicodeString(targetShip.shipName).encode();
        parameters[federateAmbassador->getParamInitiateMissileAmountFired()] = rti1516e::HLAinteger32BE(fireAmount).encode();
        parameters[federateAmbassador->getParamInitiateOrderID()] = rti1516e::HLAinteger32BE(orderID).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch.1 - Exception: " << e.what() << std::endl;
    }

    try {
        rtiAmbassador->sendInteraction(
            tempInitiateHandshake,
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent InitiateHandshake interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch.2 - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::sendFireMissileInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const Ship& targetShip, int orderID) {
    rti1516e::ParameterHandleValueMap parameters;
    try {
        parameters[federateAmbassador->getParamConfirmShooterID()] = rti1516e::HLAunicodeString(ship.shipName).encode();
        parameters[federateAmbassador->getParamConfirmShooterTeam()] = rti1516e::HLAunicodeString(ship.shipTeam).encode();
        parameters[federateAmbassador->getParamConfirmTargetID()] = rti1516e::HLAunicodeString(targetShip.shipName).encode();
        parameters[federateAmbassador->getParamConfirmMissileAmountFired()] = rti1516e::HLAinteger32BE(fireAmount).encode();
        parameters[federateAmbassador->getParamConfirmOrderID()] = rti1516e::HLAinteger32BE(orderID).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] sendFireMissileInteraction.1 - Exception: " << e.what() << std::endl;
    }

    try {
        rtiAmbassador->sendInteraction(
            tempConfirmHandshake,
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent ConfirmHandshake interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] sendFireMissileInteraction.2 - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::fireMissile(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const Ship& targetShip) {
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
        parameters[federateAmbassador->getParamTargetID()] = rti1516e::HLAunicodeString(targetShip.shipName).encode();
        parameters[federateAmbassador->getParamMissileStartPosition()] = shooterPositionRecord.encode();
        parameters[federateAmbassador->getParamMissileTargetPosition()] = targetPositionRecord.encode();
        parameters[federateAmbassador->getParamNumberOfMissilesFired()] = rti1516e::HLAinteger32BE(fireAmount).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] fireMissile.1 - Exception: " << e.what() << std::endl;
    }
  
    try {
        rtiAmbassador->sendInteraction(
            federateAmbassador->getInteractionClassFireMissile(),
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent FireMissile interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEUG] fireMissile.2 - Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::waitForExitLoop(double simulationTime, double stepsize) {
    int pulse = 0;
    std::wcout << L"[INFO] Waiting in exit loop. Simulation time: " << simulationTime << std::endl;
    try {
        //This wont work with several Ship instances
        if (federateAmbassador->getTeamStatus() == ShipTeam::BLUE && federateAmbassador->getBlueShips().empty()) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"BlueShipEmpty", rti1516e::VariableLengthData());
            while (federateAmbassador->getSyncLabel() != L"BlueShipEmpty") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
        } 
        else if (federateAmbassador->getTeamStatus() == ShipTeam::RED && federateAmbassador->getRedShips().empty()) {
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

void ShipFederate::logDetectionStart(const ShipTeam team) {
    if (team == ShipTeam::BLUE) {
        std::wcout << L"[INFO] BlueShipFederate - Detecting enemies..." << std::endl;
    } else if (team == ShipTeam::RED) {
        std::wcout << L"[INFO] RedShipFederate - Detecting enemies..." << std::endl;
    }
}

void ShipFederate::detectEnemiesForShip(Ship* ownShip, const std::vector<Ship*>& enemyShips, double maxTargetDistance) {
    Ship* closestEnemy = nullptr;
    double closestDistance = std::numeric_limits<double>::max();

    for (Ship* enemyShip : enemyShips) {
        double distance = calculateDistance(ownShip->shipPosition, enemyShip->shipPosition, 0);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestEnemy = enemyShip;
        }

        if (distance < maxTargetDistance &&
            enemyShip->currentMissilesLocking < enemyShip->maxMissilesLocking &&
            ownShip->shipNumberOfMissiles > 0) {
            federateAmbassador->closestMissileRangeToTarget.insert({ distance, { ownShip, enemyShip } });
        }
    }

    if (closestEnemy != nullptr) {
        federateAmbassador->setClosestEnemyShip(ownShip, closestEnemy);
    }
}

void ShipFederate::detectEnemies(double maxTargetDistance) {
    ShipTeam team = federateAmbassador->getTeamStatus();
    logDetectionStart(team);

    const std::vector<Ship*>& ownShips = federateAmbassador->getOwnShips();
    const std::vector<Ship*>& enemyShips = (team == ShipTeam::BLUE)
        ? federateAmbassador->getRedShips()
        : federateAmbassador->getBlueShips();

    for (Ship* ship : ownShips) {
        detectEnemiesForShip(ship, enemyShips, maxTargetDistance);
    }
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