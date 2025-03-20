#include "EnemyShipFederate.h"
#include "../include/jsonParse.h"
#include "../include/shipHelperFunctions.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(10.0, 25.0);

EnemyShipFederate::EnemyShipFederate(int instance) {
    createRTIAmbassador(instance);
}

EnemyShipFederate::~EnemyShipFederate() {
    resignFederation();
}

void startEnemyShip(int instance) {
    EnemyShipFederate EnemyShipFederate(instance);
    EnemyShipFederate.federateAmbassador->setMyShipFederateName(L"EnemyShipFederate " + std::to_wstring(instance));

    if (!EnemyShipFederate.rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {
        EnemyShipFederate.readJsonFile(instance); 
        EnemyShipFederate.connectToRTI();
        EnemyShipFederate.initializeFederation();
        EnemyShipFederate.joinFederation();
        EnemyShipFederate.waitForSyncPoint();
        EnemyShipFederate.initializeHandles();
        EnemyShipFederate.publishAttributes();
        EnemyShipFederate.subscribeInteractions();
            //Wait for setupInteraction
        EnemyShipFederate.waitForSetupSync();
            //EnemyShipFederate.registerShipObject(EnemyShipFederate.federateAmbassador->getAmountOfShips());
        EnemyShipFederate.subscribeAttributes();
        EnemyShipFederate.publishInteractions();
        EnemyShipFederate.initializeTimeFactory();
        EnemyShipFederate.enableTimeManagement();
        EnemyShipFederate.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::readJsonFile(int i) {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);
    //randomly select a ship configuration
    if (!parser.isFileOpen()) return;
    if (i > 3) {
        i = dis(gen);
    }

    parser.parseShipConfig("Ship" + std::to_string(i));
    federateAmbassador->setshipNumber(L"EnemyShip" + std::to_wstring(i));
    federateAmbassador->setshiplength(parser.getLength());
    federateAmbassador->setshipwidth(parser.getWidth());
    federateAmbassador->setshipheight(parser.getHeight());
    federateAmbassador->setNumberOfRobots(parser.getNumberOfRobots());
    std::wcout << L"Ship Number: " << federateAmbassador->getshipNumber() << std::endl;
    std::wcout << L"Ship Length: " << federateAmbassador->getshiplength() << std::endl;
    std::wcout << L"Ship Width: " << federateAmbassador->getshipwidth() << std::endl;
    std::wcout << L"Ship Height: " << federateAmbassador->getshipheight() << std::endl;
    std::wcout << L"Ship Size: " << federateAmbassador->getShipSize() << std::endl;
    std::wcout << L"Number of Robots: " << federateAmbassador->getNumberOfRobots() << std::endl;
}

void EnemyShipFederate::createRTIAmbassador(int instance) {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<EnemyShipFederateAmbassador>(rtiAmbassador.get(), instance);
}

void EnemyShipFederate::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getMyShipFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getMyShipFederateName() << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateAmbassador->getMyShipFederateName() << L" waiting for sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::initializeHandles() {

    //Initialize object handles
    federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
    federateAmbassador->setAttributeHandleMyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    federateAmbassador->setAttributeHandleMyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleMyShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Speed"));
    federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"NumberOfMissiles"));

    //Enemy ship federateName and position
    federateAmbassador->setAttributeHandleEnemyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleEnemyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    std::wcout << L"Object handles initialized" << std::endl;

    //Interaction class handles for FireRobotInteraction
    federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
    federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
    federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
    federateAmbassador->setTargetPositionParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"TargetPosition"));
    federateAmbassador->setStartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));

    //Interaction class handles for SetupInteraction
    federateAmbassador->setSetupSimulationHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
    federateAmbassador->setBlueShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfBlueShips"));
    federateAmbassador->setRedShipsParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"NumberOfRedShips"));
    federateAmbassador->setTimeScaleFactorParam(rtiAmbassador->getParameterHandle(federateAmbassador->getSetupSimulationHandle(), L"TimeScaleFactor"));

    std::wcout << L"Interaction handles initialized" << std::endl;
}

void EnemyShipFederate::publishAttributes() {
    try {
        
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleMyShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleMyShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleMyShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfMissiles());
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getMyObjectClassHandle(), attributes);
    
        std::wcout << L"Published ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::waitForSetupSync() {
    std::wcout << L"[DEBUG] federate: " << federateAmbassador->getMyShipFederateName() << L" waiting for setup sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::registerShipObject(const int& amountOfShips) {
    try {
        /*
        for (int i = 0; i < amountOfShips; i++) {
            rti1516e::ObjectInstanceHandle objectInstanceHandle = rtiAmbassador->registerObjectInstance(federateAmbassador->getMyObjectClassHandle());
            federateAmbassador->objectInstanceHandles.push_back(objectInstanceHandle);
            std::wcout << L"Registered ship object" << std::endl;

            std::wstring shipName = L"EnemyShip " + std::to_wstring(i);
            
            rti1516e::AttributeHandleValueMap attributes;
            attributes[federateAmbassador->getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(shipName).encode();
            attributes[federateAmbassador->getAttributeHandleMyShipPosition()] = rti1516e::HLAunicodeString(L"20.43829000,1562534000").encode();
            attributes[federateAmbassador->getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(speedDis(gen)).encode();
            attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfRobots()).encode();
            //Might need to change the last parameter to logical time to be able to handle in the middle of the simulation
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData());
        }
            */
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleEnemyShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleEnemyShipPosition());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getMyObjectClassHandle(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}
//Add method here to publish attributes when implemented
void EnemyShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getFireRobotHandle());
        std::wcout << L"Published interaction class: FireRobot" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getSetupSimulationHandle());
        std::wcout << L"Subscribed to SetupSimulation interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, std::wstring targetName) {

    rti1516e::ParameterHandleValueMap parameters;
    parameters[federateAmbassador->getFireRobotHandleParam()] = rti1516e::HLAinteger32BE(1).encode();
    parameters[federateAmbassador->getTargetParam()] = rti1516e::HLAunicodeString(targetName).encode();
    parameters[federateAmbassador->getTargetPositionParam()] = rti1516e::HLAunicodeString(federateAmbassador->getEnemyShipPosition()).encode();
    parameters[federateAmbassador->getStartPosRobot()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipPosition()).encode();

    try {
        rtiAmbassador->sendInteraction(
            federateAmbassador->getFireRobotHandle(), 
            parameters, 
            rti1516e::VariableLengthData(),
            logicalTimePtr);
        std::wcout << L"Sent FireRobot interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::initializeTimeFactory() {
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

void EnemyShipFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
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

void EnemyShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::runSimulationLoop() {
    Robot myShip;
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double maxTargetDistance = 8000.0; //Change when needed
    double latitude = 20.43829000;
    double longitude = 15.62534000;
    std::wstring StartShipLocation = generateShipPosition(latitude, longitude);
    bool firstTime = true;

    federateAmbassador->setMyShipPosition(StartShipLocation);

    while (simulationTime < 1.0) {
        std::cout << "Running simulation loop" << std::endl;
        //Update my values

        //Check if Logic time factory is null
        if (!logicalTimeFactory) {
            std::wcerr << L"Logical time factory is null" << std::endl;
            exit(1);
        }

        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        
        for (auto& objectInstanceHandle : federateAmbassador->objectInstanceHandles) {
            rti1516e::AttributeHandleValueMap attributes;
            attributes[federateAmbassador->getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipFederateName()).encode();
            attributes[federateAmbassador->getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(myShip.getSpeed(10, 10, 25)).encode();
            attributes[federateAmbassador->getAttributeHandleMyShipPosition()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipPosition()).encode();
            attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfRobots()).encode();
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);
        }


        if (federateAmbassador->getDistanceBetweenShips() < maxTargetDistance && !firstTime) {
            std::wcout << L"Target ship is within firing range " << std::endl;
            if (federateAmbassador->getIsFiring()) {
                std::wcout << L"Ship is already firing" << std::endl;
            }
            else {
                federateAmbassador->setIsFiring(true);
                std::wcout << std::endl << L"Firing at target" << std::endl;
                std::wcout << L"FederateName for ship to fire at: " << federateAmbassador->getEnemyShipFederateName() << std::endl;
                sendInteraction(logicalTime, 1, federateAmbassador->getEnemyShipFederateName());//Needs to be before TimeAdvanceRequest
            }
        }

        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }   

        //Stops moving towards the enemy ship when it gets close
        if (federateAmbassador->getDistanceBetweenShips() < 2000.0) { 
            federateAmbassador->setMyShipSpeed(0.0);
        }
        else {
            federateAmbassador->setMyShipSpeed(myShip.getSpeed(10, 10, 25));
        }

        federateAmbassador->setBearing(myShip.calculateInitialBearingWstring(federateAmbassador->getMyShipPosition(), federateAmbassador->getEnemyShipPosition()));
        federateAmbassador->setMyShipPosition(myShip.calculateNewPosition(federateAmbassador->getMyShipPosition(), federateAmbassador->getMyShipSpeed(), federateAmbassador->getBearing()));
        federateAmbassador->setDistanceBetweenShips(myShip.calculateDistance(federateAmbassador->getMyShipPosition(), federateAmbassador->getEnemyShipPosition(), 0));

        std::wcout << L"My ship speed: " << federateAmbassador->getMyShipSpeed() << std::endl;
        std::wcout << L"Bearing: " << federateAmbassador->getBearing() << std::endl;
        std::wcout << L"My ship position: " << federateAmbassador->getMyShipPosition() << std::endl;
        std::wcout << L"Distance between ships: " << federateAmbassador->getDistanceBetweenShips() << std::endl;

        simulationTime += stepsize;
        firstTime = false;
    }
    rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    std::wcout << L"Resigned from federation and disconnected from RTI" << std::endl;
}


int main() {
    startEnemyShip(1);
    return 0;
}