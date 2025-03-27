#include "ShootShipFederate.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(10.0, 25.0);

ShootShipFederate::ShootShipFederate() {
    createRTIAmbassador();
}


ShootShipFederate::~ShootShipFederate() {
    resignFederation();
}

void ShootShipFederate::startShootShip() {
    std::wcout << L"ShootShipFederate starting" << std::endl;
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
        initializeTimeFactory();
        enableTimeManagement();
        runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] startShootShip - Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::createRTIAmbassador() {
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        federateAmbassador = std::make_unique<MyShootShipFederateAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIAmbassador - Exception" << e.what() << std::endl;
    }
}

void ShootShipFederate::connectToRTI() {
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

void ShootShipFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Federate: " << federateName << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateName << L" waiting for sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::initializeHandles() {

    //Object Handles for publishing 
    federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
    federateAmbassador->setAttributeHandleShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
    federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
    federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
    federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
    federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));

    //Enemy ship federateName and position for subscribing
    federateAmbassador->setAttributeHandleEnemyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
    federateAmbassador->setAttributeHandleEnemyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
    std::wcout << L"Object handles initialized" << std::endl;

    //Interaction class handles for SetupInteraction
    federateAmbassador->setSetupSimulationHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
    federateAmbassador->setBlueShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfBlueShips"));
    federateAmbassador->setRedShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfRedShips"));
    federateAmbassador->setTimeScaleFactorParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"TimeScaleFactor"));
    std::wcout << L"Interaction handles initialized" << std::endl;

    federateAmbassador->setInteractionClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
    federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterID"));
    federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"Team"));
    federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterPosition"));
    federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetPosition"));
    federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"NumberOfMissilesFired"));    
    std::wcout << L"Interaction handles initialized" << std::endl;
}

void ShootShipFederate::publishAttributes() {
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

void ShootShipFederate::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleEnemyShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleEnemyShipPosition());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}
//Add method here to publish attributes when implemented
void ShootShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        std::wcout << L"Published interaction class: FireMissile" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getSetupSimulationHandle());
        std::wcout << L"Subscribed to SetupSimulation interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::waitForSetupSync() {
    std::wcout << L"[DEBUG] federate: " << federateName << L" waiting for setup sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::initializeTimeFactory() {
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

void ShootShipFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
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

void ShootShipFederate::runSimulationLoop() {
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double maxTargetDistance = 8000.0; //Change when needed
    bool firstTime = true;


    while (simulationTime < 10.0) {

        std::cout << "Running simulation loop" << std::endl;
        //Update my values

        //Check if Logic time factory is null
        if (!logicalTimeFactory) {
            std::wcerr << L"Logical time factory is null" << std::endl;
            exit(1);
        }

        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        for (const auto& [objectInstanceHandle, friendlyIndex] : federateAmbassador->friendlyShipIndexMap) {
            const Ship& ship = federateAmbassador->friendlyShips[friendlyIndex];

            std::wcout << L"Updating values for own ship instance" << std::endl;
            rti1516e::AttributeHandleValueMap attributes;

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship.shipPosition.first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(ship.shipPosition.second));

            attributes[federateAmbassador->getAttributeHandleShipFederateName()] = rti1516e::HLAunicodeString(ship.shipName).encode();
            attributes[federateAmbassador->getAttributeHandleShipSpeed()] = rti1516e::HLAfloat64BE(ship.shipSpeed).encode();
            attributes[federateAmbassador->getAttributeHandleShipPosition()] = shipPositionRecord.encode();
            attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(ship.numberOfMissiles).encode();
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);

            for (const auto& [objectInstanceHandle, enemyIndex] : federateAmbassador->enemyShipIndexMap) {
                const EnemyShip& enemyShip = federateAmbassador->enemyShips[enemyIndex];

                double distance = calculateDistance(ship.shipPosition, enemyShip.shipPosition, 0);
                shipInteractions.push_back(std::make_tuple(distance, friendlyIndex, enemyIndex, enemyShip.HP));
            }
        }

        std::sort(shipInteractions.begin(), shipInteractions.end());

        std::unordered_map<int, int> enemyShipRemainingHP;

        for (const auto& [distance, friendlyIndex, enemyIndex, missilesToFire] : shipInteractions) {
            if (federateAmbassador->enemyShips[enemyIndex].HP <= 0) {
                continue;
            }

            int fireAmount = (federateAmbassador->enemyShips[enemyIndex].HP > 0) ? missilesToFire : 0;

            if (distance < maxTargetDistance && federateAmbassador->enemyShips[enemyIndex].HP > 0) {
                sendInteraction(logicalTime, fireAmount, federateAmbassador->friendlyShips[friendlyIndex], federateAmbassador->enemyShips[enemyIndex]);
                federateAmbassador->enemyShips[enemyIndex].HP -= fireAmount;
            }
        }
        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        federateAmbassador->setBearing(180.0);

        for (const auto& [objectInstanceHandle, index] : federateAmbassador->friendlyShipIndexMap) {
            federateAmbassador->setBearing(0.0);

            Ship& ship = federateAmbassador->friendlyShips[index];
            std::wcout << L"[INFO - " << index << L"] Updating values for own ship: " << ship.shipName << std::endl;
            std::wcout << L"[INFO - " << index << L"] Ship Position: " << ship.shipPosition.first << L"," << ship.shipPosition.second << std::endl;
            std::pair<double, double> newPos = calculateNewPosition(ship.shipPosition, ship.shipSpeed, federateAmbassador->getBearing());
            ship.shipPosition = newPos;
            std::wcout << L"[INFO - " << index << L"] New ship Position: " << ship.shipPosition.first << L"," << ship.shipPosition.second << std::endl << std::endl;
            ship.shipSpeed = getSpeed(10, 10, 25);   
            std::wcout << L"[INFO - " << index << L"] Current ship speed: " << ship.shipSpeed << std::endl;
            std::wcout << L"[INFO - " << index << L"] Current number of missiles: " << ship.numberOfMissiles << std::endl;
        }

        simulationTime += stepsize;
    }
    rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    std::wcout << L"Resigned from federation and disconnected from RTI" << std::endl;
}

void ShootShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, const Ship& ship, const EnemyShip& targetShip) {
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

void ShootShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    ShootShipFederate shootShipFederate;
    shootShipFederate.startShootShip();

    return 0;
}