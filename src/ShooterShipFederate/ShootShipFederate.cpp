#include "ShootShipFederate.h"
#include "../include/jsonParse.h"
#include "../include/shipHelperFunctions.h"

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
        readJsonFile();
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
        //registerShipObject(federateAmbassador->getAmountOfShips());
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

void ShootShipFederate::readJsonFile() {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);
    //randomly select a ship configuration
    if (!parser.isFileOpen()) return;
 
      int i = dis(gen);
    

    parser.parseShipConfig("Ship" + std::to_string(i));
    federateAmbassador->setshipNumber(L"ShootShip" + std::to_wstring(i));
    federateAmbassador->setshiplength(parser.getLength());
    federateAmbassador->setshipwidth(parser.getWidth());
    federateAmbassador->setshipheight(parser.getHeight());
    federateAmbassador->setNumberOfMissiles(parser.getNumberOfMissiles());
    std::wcout << L"Ship Number: " << federateAmbassador->getshipNumber() << std::endl;
    std::wcout << L"Ship Length: " << federateAmbassador->getshiplength() << std::endl;
    std::wcout << L"Ship Width: " << federateAmbassador->getshipwidth() << std::endl;
    std::wcout << L"Ship Height: " << federateAmbassador->getshipheight() << std::endl;
    std::wcout << L"Ship Size: " << federateAmbassador->getShipSize() << std::endl;
    std::wcout << L"Number of Missiles: " << federateAmbassador->getNumberOfMissiles() << std::endl;
}

void ShootShipFederate::connectToRTI() {
    try {
        if (!rtiAmbassador) {
            std::wcerr << L"RTIambassador is null" << std::endl;
            exit(1);
        }
        federateAmbassador->setMyShipFederateName(L"ShootShipFederate");
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
        
        std::wcout << L"Published interaction class: FireRobot" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getSetupSimulationHandle());
        std::wcout << L"Subscribed to SetupSimulation interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::waitForSetupSync() {
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
    Robot myShip;
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double maxTargetDistance = 8000.0; //Change when needed
    double latitude = 20.43829000;
    double longitude = 15.62534000;
    bool firstTime = true;

    for (auto& ship : federateAmbassador->ships) {
        //ship.shipPosition = generateShipPosition(latitude, longitude);
        federateAmbassador->setMyShipPosition(generateDoubleShipPosition(latitude, longitude));
        ship.shipPosition = federateAmbassador->getMyShipPosition();
    }

    while (simulationTime < 30.0) {
        std::cout << "Running simulation loop" << std::endl;
        //Update my values

        //Check if Logic time factory is null
        if (!logicalTimeFactory) {
            std::wcerr << L"Logical time factory is null" << std::endl;
            exit(1);
        }

        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        for (const auto& [objectInstanceHandle, index] : federateAmbassador->shipIndexMap) {
            std::wcout << L"Updating values for own ship instance handle: " << objectInstanceHandle << std::endl;
            rti1516e::AttributeHandleValueMap attributes;
            //Used to get the specific ship
            const Ship& ship = federateAmbassador->ships[index];

            rti1516e::HLAfixedRecord shipPositionRecord;
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(federateAmbassador->getMyShipPosition().first));
            shipPositionRecord.appendElement(rti1516e::HLAfloat64BE(federateAmbassador->getMyShipPosition().second));

            attributes[federateAmbassador->getAttributeHandleShipFederateName()] = rti1516e::HLAunicodeString(ship.shipName).encode();
            attributes[federateAmbassador->getAttributeHandleShipTeam()] = rti1516e::HLAunicodeString(L"Blue").encode();        //Temporary value
            attributes[federateAmbassador->getAttributeHandleShipSpeed()] = rti1516e::HLAfloat64BE(myShip.getSpeed(10, 10, 25)).encode();
            attributes[federateAmbassador->getAttributeHandleShipPosition()] = shipPositionRecord.encode();
            attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfMissiles()).encode();
            rtiAmbassador->updateAttributeValues(objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);
        }

        if (federateAmbassador->getDistanceBetweenShips() < maxTargetDistance && !firstTime) {
            std::wcout << L"Target ship is within firing range" << std::endl
                << L"Distance between ships: " << federateAmbassador->getDistanceBetweenShips() << std::endl;
            if (federateAmbassador->getIsFiring()) {
                std::wcout << L"Ship is already firing" << std::endl;
            }
            else {
                federateAmbassador->setIsFiring(true);
                std::wcout << std::endl << L"Firing at target" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

        federateAmbassador->setBearing(180.0);
            // Need new 'calculateNewPosition' method
        //federateAmbassador->setMyShipPosition(myShip.calculateNewPosition(federateAmbassador->getMyShipPosition(), federateAmbassador->getMyShipSpeed(), federateAmbassador->getBearing()));
        //federateAmbassador->setDistanceBetweenShips(myShip.calculateDistance(federateAmbassador->getMyShipPosition(), federateAmbassador->getEnemyShipPosition(), 0));

        std::wcout << L"My ship speed: " << federateAmbassador->getMyShipSpeed() << std::endl;
        std::wcout << L"Bearing: " << federateAmbassador->getBearing() << std::endl;
        std::wcout << L"My ship position: " << federateAmbassador->getMyShipPosition().first << L", " << federateAmbassador->getMyShipPosition().second << std::endl;
        //::wcout << L"Distance between ships: " << federateAmbassador->getDistanceBetweenShips() << std::endl;

        simulationTime += stepsize;
        firstTime = false;
    }
    rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    std::wcout << L"Resigned from federation and disconnected from RTI" << std::endl;
}

void ShootShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, std::wstring targetName) {
/*
    rti1516e::ParameterHandleValueMap parameters;
    parameters[federateAmbassador->getFireRobotHandleParam()] = rti1516e::HLAinteger32BE(fireAmount).encode();
    parameters[federateAmbassador->getTargetParam()] = rti1516e::HLAunicodeString(targetName).encode();
    parameters[federateAmbassador->getTargetPositionParam()] = rti1516e::HLAunicodeString(federateAmbassador->getEnemyShipPosition()).encode();
    parameters[federateAmbassador->getstartPosRobot()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipPosition()).encode();

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
*/
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