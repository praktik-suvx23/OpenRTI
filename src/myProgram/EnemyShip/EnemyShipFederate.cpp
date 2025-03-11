#include "EnemyShipFederate.h"
#include "../include/jsonParse.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(10.0, 25.0);

EnemyShipFederate::EnemyShipFederate(int instance) {
    createRTIAmbassador(instance);
}

EnemyShipFederate::~EnemyShipFederate() {
    resignFederation();
}

void startShootShip(int instance) {
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
        EnemyShipFederate.registerShipObject();
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

void EnemyShipFederate::joinFederation() {
    std::wstring federationName = L"robotFederation";
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
    federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship"));
    federateAmbassador->setAttributeHandleMyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    federateAmbassador->setAttributeHandleMyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleMyShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Speed"));
    federateAmbassador->setAttributeHandleNumberOfRobots(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"NumberOfRobots"));

    //Enemy ship federateName and position
    federateAmbassador->setAttributeHandleEnemyShipFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
    federateAmbassador->setAttributeHandleEnemyShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
    std::wcout << L"Object handles initialized" << std::endl;

    //Interaction class handles
    federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
    federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
    federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
    std::wcout << L"Interaction handles initialized" << std::endl;
}

void EnemyShipFederate::publishAttributes() {
    try {
        
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleMyShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleMyShipFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleMyShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfRobots());
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getMyObjectClassHandle(), attributes);
    
        std::wcout << L"Published ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void EnemyShipFederate::registerShipObject() {
    try {
        federateAmbassador->objectInstanceHandle = rtiAmbassador->registerObjectInstance(federateAmbassador->getMyObjectClassHandle());
        std::wcout << L"Registered ship object" << std::endl;
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

void EnemyShipFederate::sendInteraction(const rti1516e::LogicalTime& logicalTimePtr, int fireAmount, std::wstring targetName) {

    rti1516e::ParameterHandleValueMap parameters;
    parameters[federateAmbassador->getFireRobotHandleParam()] = rti1516e::HLAinteger32BE(1).encode();
    parameters[federateAmbassador->getTargetParam()] = rti1516e::HLAunicodeString(targetName).encode();

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
    double latitude = 0.0;
    double longitude = 0.0;
    bool firstTime = true;

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
        attributes[federateAmbassador->getAttributeHandleMyShipFederateName()] = rti1516e::HLAunicodeString(federateAmbassador->getMyShipFederateName()).encode();
        attributes[federateAmbassador->getAttributeHandleMyShipSpeed()] = rti1516e::HLAfloat64BE(myShip.getSpeed(10, 10, 25)).encode();
        attributes[federateAmbassador->getAttributeHandleMyShipPosition()] = rti1516e::HLAunicodeString(myShip.getPosition(latitude, longitude)).encode();
        attributes[federateAmbassador->getAttributeHandleNumberOfRobots()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfRobots()).encode();
        rtiAmbassador->updateAttributeValues(federateAmbassador->objectInstanceHandle, attributes, rti1516e::VariableLengthData(), logicalTime);

        if (federateAmbassador->getDistanceBetweenShips() < maxTargetDistance && !firstTime) {
            std::wcout << L"Target ship is within firing range " << std::endl
                << L"Distance between ships: " << federateAmbassador->getDistanceBetweenShips() << std::endl;
            if (federateAmbassador->getIsFiring()) {
                std::wcout << L"Ship is already firing" << std::endl;
            }
            else {
                federateAmbassador->setIsFiring(true);
                std::wcout << std::endl << L"Firing at target" << std::endl;
                std::wcout << L"FederateName for ship to fire at: " << federateAmbassador->getEnemyShipFederateName() << std::endl;
                //sendInteraction(logicalTime, 1, federateAmbassador->getEnemyShipFederateName());//Needs to be before TimeAdvanceRequest
            }
        }

        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }   

        simulationTime += stepsize;
        firstTime = false;
    }
    rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    std::wcout << L"Resigned from federation and disconnected from RTI" << std::endl;
}


int main() {
    startShootShip(1);
    return 0;
}