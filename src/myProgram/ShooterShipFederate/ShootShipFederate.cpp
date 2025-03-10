#include "ShootShipFederate.h"
#include "../include/jsonParse.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(10.0, 25.0);

ShootShipFederate::ShootShipFederate(int instance) {
    createRTIAmbassador(instance);
}

ShootShipFederate::~ShootShipFederate() {
    resignFederation();
}

void startShootShip(int instance) {
    ShootShipFederate shootShipFederate(instance);
    shootShipFederate.federateAmbassador->setMyShipFederateName(L"ShootShipFederate " + std::to_wstring(instance));

    if (!shootShipFederate.rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {
        shootShipFederate.readJsonFile(instance);
        shootShipFederate.connectToRTI();
        shootShipFederate.initializeFederation();
        shootShipFederate.joinFederation();
        shootShipFederate.waitForSyncPoint();
        shootShipFederate.initializeHandles(); 
        shootShipFederate.publishAttributes();
        shootShipFederate.registerShipObject();
        shootShipFederate.subscribeAttributes();
        shootShipFederate.publishInteractions();
        shootShipFederate.initializeTimeFactory();
        shootShipFederate.enableTimeManagement();
        shootShipFederate.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::readJsonFile(int i) {
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
    federateAmbassador->setshipNumber(L"ShootShip" + std::to_wstring(i));
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

void ShootShipFederate::createRTIAmbassador(int instance) {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MyShootShipFederateAmbassador>(rtiAmbassador.get(), instance);
}

void ShootShipFederate::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::initializeFederation() {
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/robot.xml"};
    std::wstring mimModule = L"foms/MIM.xml";

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
    std::wstring federationName = L"robotFederation";
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getMyShipFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getMyShipFederateName() << L" - joined federation: " << federationName << std::endl;
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
    federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship"));
    federateAmbassador->setAttributeHandleMyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    federateAmbassador->setAttributeHandleMyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleMyShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Speed"));
    federateAmbassador->setAttributeHandleNumberOfRobots(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"NumberOfRobots"));

    //Enemy ship federateName and position for subscribing
    federateAmbassador->setAttributeHandleEnemyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleEnemyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    std::wcout << L"Object handles initialized" << std::endl;

    //Interaction Handles
    federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
    federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
    federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
    federateAmbassador->setstartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));
    std::wcout << L"Interaction handles initialized" << std::endl;
}

void ShootShipFederate::publishAttributes() {
    try {
        //federateAmbassador->getAttributeHandleFutureShipPosition(), add this later? into publish
        //Also add shipSize here later
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getMyObjectClassHandle(), {
            federateAmbassador->getAttributeHandleMyShipPosition(),
            federateAmbassador->getAttributeHandleMyShipSpeed(),
            federateAmbassador->getAttributeHandleMyShipFederateName(),
            federateAmbassador->getAttributeHandleNumberOfRobots()
        });
        std::wcout << L"Published ship with attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::registerShipObject() {
    try {
        federateAmbassador->objectInstanceHandle = rtiAmbassador->registerObjectInstance(federateAmbassador->getMyObjectClassHandle());
        std::wcout << L"Registered ship object" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::subscribeAttributes() {
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
void ShootShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getFireRobotHandle());
        std::wcout << L"Published interaction class: FireRobot" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, std::wstring targetName) {

    rti1516e::ParameterHandleValueMap parameters;
    parameters[federateAmbassador->getFireRobotHandleParam()] = rti1516e::HLAinteger32BE(fireAmount).encode();
    parameters[federateAmbassador->getTargetParam()] = rti1516e::HLAunicodeString(targetName).encode();
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

void ShootShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void ShootShipFederate::runSimulationLoop() {
    Robot myShip;
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();
    double simulationTime = 0.0;
    double stepsize = 0.5;
    double maxTargetDistance = 8000.0; //Change when needed
    double latitude = 0.0;
    double longitude = 0.0;

    federateAmbassador->setMyShipPosition(myShip.getPosition(latitude, longitude));

    while (simulationTime < 3.0) {
        std::cout << "Running simulation loop" << std::endl;
        //Update my values

        //Check if Logic time factory is null
        if (!logicalTimeFactory) {
            std::wcerr << L"Logical time factory is null" << std::endl;
            exit(1);
        }

        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

        rti1516e::AttributeHandleValueMap attributes;
        attributes[federateAmbassador->getAttributeHandleMyShipPosition()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipPosition()).encode();
        attributes[federateAmbassador->getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(federateAmbassador->getMyShipSpeed()).encode();
        attributes[federateAmbassador->getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipFederateName()).encode();
        attributes[federateAmbassador->getAttributeHandleNumberOfRobots()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfRobots()).encode();

        rtiAmbassador->updateAttributeValues(
            federateAmbassador->objectInstanceHandle, 
            attributes, 
            rti1516e::VariableLengthData(),
            logicalTime
        );


        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }        

        std::wcout << L"My ship position" << federateAmbassador->getMyShipPosition() << std::endl;
        std::wcout << L"Enemy ship position" << federateAmbassador->getEnemyShipPosition() << std::endl;

        if (federateAmbassador->getDistanceBetweenShips() < maxTargetDistance) {
            std::wcout << L"Target ship is within firing range" << std::endl
                << L"Distance between ships: " << federateAmbassador->getDistanceBetweenShips() << std::endl;
            if (federateAmbassador->getIsFiring()) {
                std::wcout << L"Ship is already firing" << std::endl;
            }
            else {
                federateAmbassador->setIsFiring(true);
                std::wcout << std::endl << L"Firing at target" << std::endl;
                std::wcout << L"FederateName for ship to fire at: " << federateAmbassador->getEnemyShipFederateName() << std::endl;
                //Add Send interaction here
                //Fire at the target
                std::wcout << L"Sending FireRobot interaction with enemyFederateName: " <<federateAmbassador->getMyShipFederateName() << std::endl;
                sendInteraction(logicalTime, 1, federateAmbassador->getEnemyShipFederateName());//Needs to be before TimeAdvanceRequest

            }
        }
        simulationTime += stepsize;
    }
    rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    std::wcout << L"Resigned from federation and disconnected from RTI" << std::endl;
}

int main() {
    startShootShip(1);
    return 0;
}