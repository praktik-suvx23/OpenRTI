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
    federateAmbassador->setLogType(federateAmbassador->getTeamStatus() == ShipTeam::BLUE ? loggingType::LOGGING_BLUESHIP : loggingType::LOGGING_REDSHIP);
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
        federateAmbassador->setInteractionClassConfirmBlueHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.ConfirmHandshakeBlue"));
        federateAmbassador->setInteractionClassInitiateBlueHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeBlue"));
        tempConfirmHandshake = federateAmbassador->getInteractionClassConfirmBlueHandshake();
        tempInitiateHandshake = federateAmbassador->getInteractionClassInitiateBlueHandshake();
    } else if (federateAmbassador->getTeamStatus() == ShipTeam::RED) {
        federateAmbassador->setInteractionClassConfirmRedHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.ConfirmHandshakeRed"));
        federateAmbassador->setInteractionClassInitiateRedHandshake(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.InitiateHandshakeRed"));
        tempConfirmHandshake = federateAmbassador->getInteractionClassConfirmRedHandshake();
        tempInitiateHandshake = federateAmbassador->getInteractionClassInitiateRedHandshake();
    } else {
        std::wcerr << L"[DEBUG] Error: Team status not set correctly" << std::endl;
        resignFederation();
    }
    //Interaction class handles for InitiateHandshake
    federateAmbassador->setParamInitiateShooterID(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"ShooterID"));
    federateAmbassador->setParamInitiateMissileAmountFired(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"ShooterMissilesAvailable"));
    federateAmbassador->setParamInitiateTargetID(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"TargetID"));
    federateAmbassador->setParamInitiateMaxMissilesRequired(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"TargetMaxMissilesRequired"));
    federateAmbassador->setParamInitiateMissilesCurrentlyTargeting(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"TargetMissilesCurrentlyTargeting"));
    federateAmbassador->setParamInitiateDistanceToTarget(rtiAmbassador->getParameterHandle(tempInitiateHandshake, L"DistanceToTarget"));
    
    std::wcout << L"InteractionClassInitiateHandshake: " << tempInitiateHandshake << " initialized" << std::endl;

    //Interaction class handles for ConfirmHandshake
    federateAmbassador->setParamConfirmShooterID(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"ShooterID"));
    federateAmbassador->setParamConfirmMissileAmountFired(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"ShooterMissilesLoaded"));
    federateAmbassador->setParamConfirmTargetID(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"TargetID"));
    federateAmbassador->setParamConfirmAllowFire(rtiAmbassador->getParameterHandle(tempConfirmHandshake, L"AllowedFire"));

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
        //rtiAmbassador->publishInteractionClass(tempConfirmHandshake);
        std::wcout << L"Published interaction class: FireMissile" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        //rtiAmbassador->subscribeInteractionClass(tempInitiateHandshake);
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

        if (federateAmbassador->getTeamStatus() == ShipTeam::BLUE) {
            rtiAmbassador->registerFederationSynchronizationPoint(L"BlueShipReady", rti1516e::VariableLengthData());

            while (federateAmbassador->getSyncLabel() != L"BlueShipReady") {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
            while (federateAmbassador->getSyncLabel() != L"RedShipReady") {
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
    
    
    initializeLogFile(federateAmbassador->getLogType());
    logToFile("[INFO] New run begun...", federateAmbassador->getLogType());

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

            std::wstring senderName = L"Ship";
            std::string senderUtf8(senderName.begin(), senderName.end()); // naive UTF-8 conversion
            rti1516e::VariableLengthData tag(senderUtf8.data(), senderUtf8.size());
    
            rtiAmbassador->updateAttributeValues(ship->objectInstanceHandle, attributes, tag, logicalTime);
        }

        // === Enemy Detection & Targeting ===
        if (!federateAmbassador->getClosestEnemyShip().empty()) {
            federateAmbassador->clearClosestEnemyShip();
        }
        detectEnemies(maxTargetDistance);
    
        // === Fire Missiles at Targets ===
        for (FireOrder& order : federateAmbassador->getFireOrders()) {
            if (order.status != ORDER_INITIATED) continue;
            order.status = ORDER_IN_PROGRESS;
            Ship* ship = order.shooterShip;
            Ship* enemyShip = order.targetShip;

            if (!ship || !enemyShip) continue;

            if (ship->shipName[0] == enemyShip->shipName[0]) continue;

            int availableLocking = enemyShip->maxMissilesLocking - enemyShip->currentMissilesLocking;
            if (availableLocking <= 0) continue;

            int maxMissilesToFire = std::min({ ship->shipNumberOfMissiles, availableLocking, order.missileAmount });
            if (maxMissilesToFire <= 0) continue;

            if (enemyShip->currentMissilesLocking > enemyShip->maxMissilesLocking) {
                std::wcout << L"[ERROR] " << enemyShip->shipName << L" already has "
                        << enemyShip->currentMissilesLocking 
                        << L" missile(s) locking on it, which is its maximum of "
                        << enemyShip->maxMissilesLocking << L"." << std::endl;
                continue;
            }

            std::wstring logWmessage = L"[PREPARE MISSILE] " + ship->shipName + L" has " + std::to_wstring(ship->shipNumberOfMissiles)
                                    + L" missile(s) and prepares to fire at: " + enemyShip->shipName + L" with "
                                    + std::to_wstring(maxMissilesToFire) + L"/" 
                                    + std::to_wstring(enemyShip->maxMissilesLocking) + L" missile(s) locking on it.";
            wstringToLog(logWmessage, federateAmbassador->getLogType());

            if ((federateAmbassador->getTeamStatus() == ShipTeam::BLUE && !federateAmbassador->getBlueShips().empty()) ||
                (federateAmbassador->getTeamStatus() == ShipTeam::RED && !federateAmbassador->getRedShips().empty())) {

                double distance = calculateDistance(ship->shipPosition, enemyShip->shipPosition, 0);
                prepareMissileLaunch(logicalTime, maxMissilesToFire, distance, *ship, *enemyShip);
            } else {
                enemyShip->currentMissilesLocking += maxMissilesToFire;
                ship->shipNumberOfMissiles -= maxMissilesToFire;
                fireMissile(logicalTime, maxMissilesToFire, *ship, *enemyShip);
            }

            // Optional bookkeeping
            tempLockingCount[enemyShip] += maxMissilesToFire;
        }
    
        // === Move Ships Based on Closest Enemy ===
        for (auto& [ship, enemyShip] : federateAmbassador->getClosestEnemyShip()) {
            if (ship->shipTeam == enemyShip->shipTeam) {
                std::wcout << L"[INFO] Skipping friendly fire from " << ship->shipName << L" to " << enemyShip->shipName << std::endl;
                continue;
            }
            ship->bearing = calculateBearing(ship->shipPosition, enemyShip->shipPosition);
            ship->shipSpeed = getSpeed(10, 10, 25);
        }
        for (auto& ship : federateAmbassador->getOwnShips()) {
            ship->shipPosition = calculateNewPosition(ship->shipPosition, ship->shipSpeed, ship->bearing);
        }

        for (FireOrder& order : federateAmbassador->getFireOrders()) {
            if (order.status != ORDER_CONFIRMED) continue;
            order.status = ORDER_COMPLETED;
            logWmessage = L"[RECEIVED FIREORDER] " + order.shooterShip->shipName + L" have " + std::to_wstring(order.missileAmount)
                        + L" missile(s) and fire at: " + order.targetShip->shipName + L" with " + std::to_wstring(order.missileAmount) 
                        + L" missile(s) locking on it.";
            wstringToLog(logWmessage, federateAmbassador->getLogType());
            fireMissile(logicalTime, order.missileAmount, *order.shooterShip, *order.targetShip);
        }
    
        federateAmbassador->closestMissileRangeToTarget.clear();
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
            rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION); //Might need to be moved

            //waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
        if ((federateAmbassador->getSyncLabel() == L"RedShipEmpty" && federateAmbassador->getTeamStatus() == ShipTeam::BLUE) ||
            (federateAmbassador->getSyncLabel() == L"BlueShipEmpty" && federateAmbassador->getTeamStatus() == ShipTeam::RED)) {
            rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION); //Might need to be moved

            //waitForExitLoop(simulationTime, stepsize);
            break;
        }
    
    } while (federateAmbassador->getSyncLabel() != L"ReadyToExit");    

}

void ShipFederate::prepareMissileLaunch(const rti1516e::LogicalTime& logicalTimePtr, const int fireAmount, const double distance, const Ship& ship, const Ship& targetShip) {
    rti1516e::ParameterHandleValueMap parameters;
    try {
        std::wcout << L"[DEBUG] Data being sent: " << ship.shipName << L" - " << ship.shipTeam << L" - " << targetShip.shipName << std::endl;
        parameters[federateAmbassador->getParamInitiateShooterID()] = rti1516e::HLAunicodeString(ship.shipName).encode();
        parameters[federateAmbassador->getParamInitiateMissileAmountFired()] = rti1516e::HLAinteger32BE(ship.shipNumberOfMissiles).encode();
        parameters[federateAmbassador->getParamInitiateTargetID()] = rti1516e::HLAunicodeString(targetShip.shipName).encode();
        parameters[federateAmbassador->getParamInitiateMaxMissilesRequired()] = rti1516e::HLAinteger32BE(targetShip.maxMissilesLocking).encode();
        parameters[federateAmbassador->getParamInitiateMissilesCurrentlyTargeting()] = rti1516e::HLAinteger32BE(targetShip.currentMissilesLocking).encode();
        parameters[federateAmbassador->getParamInitiateDistanceToTarget()] = rti1516e::HLAfloat64BE(distance).encode();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch.1 - Exception: " << e.what() << std::endl;
    }

    try {
        rtiAmbassador->sendInteraction(
            tempInitiateHandshake,
            parameters,
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"[INFO] Sent InitiateHandshake interaction over: " << tempInitiateHandshake << std::endl;

        logWmessage = L"[PREPARE MISSILE]";
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] prepareMissileLaunch.2 - Exception: " << e.what() << std::endl;
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
        rtiAmbassador->sendInteraction( //Does not use time for the moment
            federateAmbassador->getInteractionClassFireMissile(),
            parameters,
            rti1516e::VariableLengthData());
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

void ShipFederate::detectEnemiesForShip(const Ship* ownShip, const std::vector<Ship*>& enemyShips, const double maxTargetDistance) {
    Ship* closestEnemy = nullptr;
    double closestDistance = std::numeric_limits<double>::max();

    for (auto& enemyShip : enemyShips) {
        double distance = calculateDistance(ownShip->shipPosition, enemyShip->shipPosition, 0);

        if (distance < closestDistance) {
            closestDistance = distance;
            closestEnemy = enemyShip;
        }

        if (enemyShip->currentMissilesLocking >= enemyShip->maxMissilesLocking) {
            continue;
        }

        if (distance < maxTargetDistance &&
            enemyShip->currentMissilesLocking < enemyShip->maxMissilesLocking &&
            ownShip->shipNumberOfMissiles > 0) {

            // Create a FireOrder with 1 missile and default ORDER_UNASSIGNED status
            FireOrder order(const_cast<Ship*>(ownShip), enemyShip, 1, ORDER_INITIATED);

            federateAmbassador->addFireOrder(order);
        }
    }

    if (closestEnemy != nullptr) {
        federateAmbassador->setClosestEnemyShip(const_cast<Ship*>(ownShip), closestEnemy);
    }
}

void ShipFederate::detectEnemies(double maxTargetDistance) {
    ShipTeam team = federateAmbassador->getTeamStatus();
    logDetectionStart(team);

    const std::vector<Ship*>& ownShips = federateAmbassador->getOwnShips();
    const std::vector<Ship*>& enemyShips = (team == ShipTeam::BLUE)
        ? federateAmbassador->getRedShips()
        : federateAmbassador->getBlueShips();

    for (const Ship* ship : ownShips) {
        bool hasActiveOrder = false;
        for (const FireOrder& order : federateAmbassador->getFireOrders()) {
            if (order.shooterShip == ship) {
                if (order.status != ORDER_UNASSIGNED && order.status != ORDER_CANCELLED) {
                    hasActiveOrder = true;
                    break;
                }
            }
        }
        if (!hasActiveOrder) {
            detectEnemiesForShip(ship, enemyShips, maxTargetDistance);
        }
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