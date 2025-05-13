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
    //initializeFederation();
    joinFederation();
    std::wcout << L"-----------------------" << std::endl;
    initializeHandles();
    std::wcout << L"-----------------------" << std::endl;
    subscribeAttributes();
    std::wcout << L"-----------------------" << std::endl;
    publishAttributes();
    std::wcout << L"-----------------------" << std::endl;
    subscribeInteractions();
    std::wcout << L"-----------------------" << std::endl;
    publishInteractions();
    std::wcout << L"-----------------------" << std::endl;
    setupMissileVisualization();
    std::wcout << L"-----------------------" << std::endl;
    //waitForSetupSync();
    initializeTimeFactory();
    std::wcout << L"-----------------------" << std::endl;
    enableTimeManagement();
    std::wcout << L"-----------------------" << std::endl;
    readyCheck();
    std::wcout << L"-----------------------" << std::endl;
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
        federateAmbassador->setAttributeHandleFutureShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FuturePosition"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));

        // For setup interaction class TargetHit and its parameters. Subscribe
        federateAmbassador->setInteractionClassTargetHit(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.TargetHit"));
        federateAmbassador->setParamTargetHitID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetID"));
        federateAmbassador->setParamTargetHitTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetTeam"));
        federateAmbassador->setParamTargetHitPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetPosition"));
        federateAmbassador->setParamTargetHitDestroyed(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassTargetHit(), L"TargetDestroyed"));
     
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
        attributes.insert(federateAmbassador->getAttributeHandleFutureShipPosition());
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
    bool heightAchieved = false;
    double stepsize = 0.5;
    double simulationTime = federateAmbassador->getCurrentLogicalTime();

    //Logic here to create new missile federates with the help of the missile objects
   while (federateAmbassador->getSyncLabel() != L"ReadyToExit") {          // Loop if no missiles are fired. Improve this 'true' condition
        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);

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

        //Add logic and calculations here for missile movement

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
    std::wofstream outFile(DATA_LOG_PATH, std::ios::trunc); //See Data_LOG_PATH in CMakeLists.txt
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