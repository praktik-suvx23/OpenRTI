#include "RobotFederate.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

RobotFederate::RobotFederate(int instance) 
: gen(rd()), speedDis(250.0, 350.0) {
    createRTIAmbassador(instance);
}

RobotFederate::~RobotFederate() {
    resignFederation();
}

void startRobotSubscriber(int instance) {
    RobotFederate robotFederate(instance);
    robotFederate.federateAmbassador->setFederateName(L"RobotFederate " + std::to_wstring(instance));

    if(!robotFederate.rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {
        robotFederate.connectToRTI();
        robotFederate.initializeFederation();
        robotFederate.joinFederation();
        robotFederate.waitForSyncPoint();
        robotFederate.initializeHandles();
        robotFederate.subscribeAttributes();
        robotFederate.subscribeInteractions();
        robotFederate.initializeTimeFactory();
        robotFederate.enableTimeManagement();
        robotFederate.runSimulationLoop();
    } 
    catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::createRTIAmbassador(int instance) {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MyFederateAmbassador>(rtiAmbassador.get(), instance);
}

void RobotFederate::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::initializeFederation() {
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

void RobotFederate::joinFederation() {
    std::wstring federationName = L"robotFederation";
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getFederateName() << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateAmbassador->getFederateName() << L" waiting for sync point" << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::initializeHandles() {
    try {
        //Adjust accordingly of the attributes you want to subscribe to
        federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship"));
        federateAmbassador->setAttributeHandleShipTag(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Ship-tag"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
        federateAmbassador->setAttributeHandleFutureShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FuturePosition"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Speed"));
        federateAmbassador->setAttributeHandleFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfRobots(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"NumberOfRobots"));
        std::wcout << L"Object handles initialized" << std::endl;

        federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
        federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
        federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
        federateAmbassador->setStartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));
        
        //std::wcout << L"Interaction handles initialized" << std::endl;


    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::subscribeAttributes() {
    try {
        //Adjust accordingly of the attributes you want to subscribe to
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipTag());
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleFutureShipPosition());
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed());
        attributes.insert(federateAmbassador->getAttributeHandleFederateName());
        attributes.insert(federateAmbassador->getAttributeHandleShipSize());
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfRobots());
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getMyObjectClassHandle(), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getFireRobotHandle());
        std::wcout << L"Subscribed to fire interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::initializeTimeFactory() {
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

void RobotFederate::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
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

void RobotFederate::runSimulationLoop() { //The main simulation loop

    
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();

    //initial values
    double stepsize = 0.5;
    double simulationTime = 0.0;
    bool heightAchieved = false;
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;

    //federateAmbassador->setCurrentPosition(federateAmbassador->_robot.getPosition(currentLatitude, currentLongitude));
    while (simulationTime < 150.0) { //Change this condition to hit when implemented, for now uses a timeout
        //updating values, make this to a function

    
        if(federateAmbassador->startFire) {
            federateAmbassador->setCurrentSpeed(federateAmbassador->_robot.getSpeed(federateAmbassador->getCurrentSpeed(), 250.0, 450.0));
            federateAmbassador->setCurrentFuelLevel(federateAmbassador->_robot.getFuelLevel(federateAmbassador->getCurrentSpeed()));

            if (!heightAchieved) {
                federateAmbassador->setCurrentAltitude(federateAmbassador->_robot.getAltitude());
                if (federateAmbassador->getCurrentAltitude() >= 1000.0) {
                    federateAmbassador->setCurrentAltitude(1000.0);
                    heightAchieved = true;
                }
            }
            if (heightAchieved) {
                federateAmbassador->setCurrentAltitude(federateAmbassador->_robot.reduceAltitude(
                federateAmbassador->getCurrentAltitude(), 
                federateAmbassador->getCurrentSpeed(), 
                federateAmbassador->getCurrentDistance())
                );
            }
        }
        
        //--------------------------------------------------------

        //logical time
        if (!logicalTimeFactory) {
            std::wcerr << L"Logical time factory is null" << std::endl;
            exit(1);
        }

        rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
        federateAmbassador->isAdvancing = true;
        rtiAmbassador->timeAdvanceRequest(logicalTime);

        while (federateAmbassador->isAdvancing) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        simulationTime += stepsize; //Makes the simulation time advance
    }
}

void RobotFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 1;
    std::wofstream outFile(DATA_LOG_PATH, std::ios::trunc); //See Data_LOG_PATH in CMakeLists.txt

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startRobotSubscriber, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    return 0;
}