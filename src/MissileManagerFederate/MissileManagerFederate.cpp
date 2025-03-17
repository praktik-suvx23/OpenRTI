#include "MissileManagerFederate.h"
#include "include/MissileManagerHelper.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

MissileManager::MissileManager() {
    createRTIAmbassador();
}

MissileManager::~MissileManager() {
    resignFederation();
}

void MissileManager::startMissileManager() {
    federateAmbassador->setFederateName(L"MissileManager");

    if(!rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {
        connectToRTI();
        initializeFederation();
        joinFederation();
        waitForSyncPoint();
        initializeHandles();
        subscribeInteractions();
        initializeTimeFactory();
        enableTimeManagement();
        runSimulationLoop();
    } 
    catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::createRTIAmbassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MissileManagerAmbassador>(rtiAmbassador.get());
}

void MissileManager::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::initializeFederation() {
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

void MissileManager::joinFederation() {
    std::wstring federationName = L"robotFederation";
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getFederateName() << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::waitForSyncPoint() {
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

void MissileManager::initializeHandles() {
    try {
<<<<<<< HEAD:src/MissileManagerFederate/MissileManagerFederate.cpp
        // Subscribe to interaction class and parameters for FireMissile interaction
        MissileManagerSetter::setFireMissileHandle(*federateAmbassador, rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
        MissileManagerSetter::setShooterIDParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"ShooterID"));
        MissileManagerSetter::setTargetIDParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"TargetID"));
        MissileManagerSetter::setShooterPositionParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"ShooterPosition"));
        MissileManagerSetter::setTargetPositionParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"TargetPosition"));
        MissileManagerSetter::setMissileCountParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"MissileCount"));
        MissileManagerSetter::setMissileTypeParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"MissileType"));
        MissileManagerSetter::setMaxDistanceParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"MaxDistance"));
        MissileManagerSetter::setMissileSpeedParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"MissileSpeed"));
        MissileManagerSetter::setLockOnDistanceParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"LockOnDistance"));
        MissileManagerSetter::setFireTimeParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(MissileManagerGetter::getFireMissileHandle(*federateAmbassador), L"FireTime"));
=======
        //Adjust accordingly of the attributes you want to subscribe to
        federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship"));
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
        federateAmbassador->setTargetPositionParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"TargetPosition"));
        federateAmbassador->setStartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));
>>>>>>> origin/mergeIntoConfirmHit:src/myProgram/RobotFederate/RobotFederate.cpp
        
        std::wcout << L"Interaction handles initialized" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(MissileManagerGetter::getFireMissileHandle(*federateAmbassador));
        std::wcout << L"Subscribed to FireMissile interaction" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::initializeTimeFactory() {
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

void MissileManager::enableTimeManagement() { //Must work and be called after InitializeTimeFactory
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

void MissileManager::runSimulationLoop() {
    /* Shouldn't be 'true' in the final implementation.
        Sync point where if ships shut down -> shut down MissileManager? */
    while(true) {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);

        if (MissileManagerGetter::getFlagActiveMissile(*federateAmbassador)) {
            std::pair<double, double> shooterPosition = MissileManagerGetter::getShooterPosition(*federateAmbassador);
            std::pair<double, double> targetPosition = MissileManagerGetter::getTargetPosition(*federateAmbassador);
            double missileSpeed = MissileManagerGetter::getMissileSpeed(*federateAmbassador);
            int missileCount = MissileManagerGetter::getMissileCount(*federateAmbassador);

            for (int i = 0; i < missileCount; i++) {
                ClassActiveMissile missile(i, shooterPosition.first, shooterPosition.second, targetPosition.first, targetPosition.second, missileSpeed);
                missile.launch();
                federateAmbassador->activeMissiles.push_back(std::move(missile));
            }

            MissileManagerSetter::setFlagActiveMissile(*federateAmbassador, false);
        }

        federateAmbassador->activeMissiles.erase(
            std::remove_if(federateAmbassador->activeMissiles.begin(), federateAmbassador->activeMissiles.end(),
                [](const ClassActiveMissile& missile) { return !missile.isFlyingStatus(); }
            ),
            federateAmbassador->activeMissiles.end()
        );
    }
/*
This code will be used in 'ActiveMissile' class instead. This is just a placeholder for now.


    while(!federateAmbassador->startFire) {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
    
    federateAmbassador->startTime = std::chrono::high_resolution_clock::now();

    //initial values
    double stepsize = 0.5;
    double simulationTime = 0.0;
    bool heightAchieved = false;

    int i = 0;
    bool firstTime = true;

    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    double initialBearing = 0.0;

    federateAmbassador->setCurrentPosition(federateAmbassador->_robot.getPosition(currentLatitude, currentLongitude));
    while (!federateAmbassador->getHitStatus()) { //Change this condition to hit when implemented, for now uses a timeout
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

        //Add calculations here
        if (federateAmbassador->startFire) {
            initialBearing = federateAmbassador->_robot.calculateInitialBearingWstring(federateAmbassador->getCurrentPosition(), federateAmbassador->getShipPosition());
            federateAmbassador->setCurrentPosition(federateAmbassador->_robot.calculateNewPosition(federateAmbassador->getCurrentPosition(), federateAmbassador->getCurrentSpeed(), initialBearing));
            federateAmbassador->setCurrentDistance(federateAmbassador->_robot.calculateDistance(federateAmbassador->getCurrentPosition(), federateAmbassador->getShipPosition(), federateAmbassador->getCurrentAltitude()));
            federateAmbassador->setCurrentDistance(federateAmbassador->_robot.calculateDistance(federateAmbassador->getCurrentPosition(), federateAmbassador->getShipPosition(), federateAmbassador->getCurrentAltitude()));

            std::wcout << std::endl << L"Instance " << federateAmbassador->instance << L": Robot Current Position: " << federateAmbassador->getCurrentPosition() << std::endl;
            std::wcout << L"Instance " << federateAmbassador->instance << L": Ship Current Position: " << federateAmbassador->getShipPosition() << std::endl;
            std::wcout << L"Instance " << federateAmbassador->instance << L": Robot Current Altitude: " << federateAmbassador->getCurrentAltitude() << std::endl;
            std::wcout << L"Instance " << federateAmbassador->instance << L": Distance between robot and ship: " << federateAmbassador->getCurrentDistance() << " meters" << std::endl;


            //print values to log file
            if (federateAmbassador->getCurrentDistance() < 50) {
                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> realTimeDuration = endTime - federateAmbassador->startTime;
                double realTime = realTimeDuration.count();
                const auto& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(logicalTime);
                double federateSimulationTime = floatTime.getTime();

                std::vector<std::wstring> finalData;
                finalData.push_back(L"--------------------------------------------");
                finalData.push_back(L"Instance : " + std::to_wstring(federateAmbassador->instance));
                finalData.push_back(L"Last Distance : " + std::to_wstring(federateAmbassador->getCurrentDistance()) + L" meters");
                finalData.push_back(L"Last Altitude : " + std::to_wstring(federateAmbassador->getCurrentAltitude()) + L" meters");
                finalData.push_back(L"Last Speed : " + std::to_wstring(federateAmbassador->getCurrentSpeed()) + L" m/s");
                finalData.push_back(L"Last position for robot : " + federateAmbassador->getCurrentPosition());
                finalData.push_back(L"Last position for ship : " + federateAmbassador->getShipPosition());
                finalData.push_back(L"Simulation time : " + std::to_wstring(federateSimulationTime) + L" seconds");
                finalData.push_back(L"Real time : " + std::to_wstring(realTime) + L" seconds");
                finalData.push_back(L"--------------------------------------------");

                std::ofstream outFile;
                outFile.open(DATA_LOG_PATH, std::ios::app);
                if (outFile.is_open()) {
                    for (const auto& entry : finalData) {
                        outFile << std::string(entry.begin(), entry.end()) << std::endl;
                    }
                    outFile.close();
                    std::wcout << L"Data successfully written to finalData.txt" << std::endl;
                } else {
                    std::wcerr << L"Unable to open file: finalData.txt" << std::endl;
                }
                std::wcout << L"Target reached" << std::endl;
                federateAmbassador->setCurrentDistance(federateAmbassador->_robot.calculateDistance(federateAmbassador->getCurrentPosition(), federateAmbassador->getShipPosition(), federateAmbassador->getCurrentAltitude()));
                std::wcout << L"Instance " << federateAmbassador->instance << L": Distance between robot and ship before last contact: " << federateAmbassador->getCurrentDistance() << " meters" << std::endl;
                rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
            }
        }
        simulationTime += stepsize; //Makes the simulation time advance
    }
*/
}

void MissileManager::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    std::wofstream outFile(DATA_LOG_PATH, std::ios::trunc); //See Data_LOG_PATH in CMakeLists.txt

    MissileManager missileManager;
    missileManager.startMissileManager();

    return 0;
}