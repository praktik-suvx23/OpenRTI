#include "MissileManagerFederate.h"

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
        // Subscribe to interaction class and parameters for FireMissile interaction
        federateAmbassador->setFireMissileHandle(*federateAmbassador, rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
        federateAmbassador->setShooterIDParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"ShooterID"));
        federateAmbassador->setTargetIDParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"TargetID"));
        federateAmbassador->setShooterPositionParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"ShooterPosition"));
        federateAmbassador->setTargetPositionParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"TargetPosition"));
        federateAmbassador->setMissileCountParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"MissileCount"));
        //federateAmbassador->setMissileTypeParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"MissileType"));
        //federateAmbassador->setMaxDistanceParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"MaxDistance"));
        //federateAmbassador->setMissileSpeedParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"MissileSpeed"));
        federateAmbassador->setLockOnDistanceParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"LockOnDistance"));
        federateAmbassador->setFireTimeParamHandle(*federateAmbassador, rtiAmbassador->getParameterHandle(federateAmbassador->getFireMissileHandle(*federateAmbassador), L"FireTime"));

        // Initialize object class and attribute handles for Ship object
        federateAmbassador->getShipClassHandle(*federateAmbassador, rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->getAttributeHandleShipID(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"ShipID"));
        federateAmbassador->getAttributeHandleShipTeam(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"ShipTeam"));
        federateAmbassador->getAttributeHandleShipPosition(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"Position"));
        federateAmbassador->getAttributeHandleShipSpeed(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"Speed"));
        federateAmbassador->getAttributeHandleShipSize(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"ShipSize"));
        //MissileMangerSetter::getAttributeHandleShipAngle(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"Angle"));
        federateAmbassador->getAttributeHandleNumberOfMissiles(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getShipClassHandle(*federateAmbassador), L"NumberOfMissiles"));

        // Initialize object class and attribute handles for Missile object
        federateAmbassador->getMissileClassHandle(*federateAmbassador, rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Missile"));
        federateAmbassador->getAttributeHandleMissileID(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"MissileID"));
        //federateAmbassador->getAttributeHandleSpeed(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"Speed"));
        //federateAmbassador->getAttributeHandleFuelLevel(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"FuelLevel"));
        federateAmbassador->getAttributeHandlePosition(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"Position"));
        federateAmbassador->getAttributeHandleAltitude(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"Altitude"));
        //federateAmbassador->getAttributeHandleTargetID(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"TargetID"));
        federateAmbassador->getAttributeHandleMissileTeam(*federateAmbassador, rtiAmbassador->getAttributeHandle(federateAmbassador->getMissileClassHandle(*federateAmbassador), L"MissileTeam"));
        
        std::wcout << L"Interaction handles initialized" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::publishAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleMissileID(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandlePosition(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleAltitude(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleMissileTeam(*federateAmbassador));

        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getMissileClassHandle(*federateAmbassador), attributes);
        std::wcout << L"Published missile attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->getAttributeHandleShipID(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleShipTeam(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleShipPosition(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleShipSpeed(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleShipSize(*federateAmbassador));
        attributes.insert(federateAmbassador->getAttributeHandleNumberOfMissiles(*federateAmbassador));
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->getShipClassHandle(*federateAmbassador), attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileManager::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getFireMissileHandle(*federateAmbassador));
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

        if (federateAmbassador->getFlagActiveMissile(*federateAmbassador)) {
            std::pair<double, double> shooterPosition = federateAmbassador->getShooterPosition(*federateAmbassador);
            std::pair<double, double> targetPosition = federateAmbassador->getTargetPosition(*federateAmbassador);
            double missileSpeed = federateAmbassador->getMissileSpeed(*federateAmbassador);
            int missileCount = federateAmbassador->getMissileCount(*federateAmbassador);

            for (int i = 0; i < missileCount; i++) {
                ClassActiveMissile missile(i, shooterPosition.first, shooterPosition.second, targetPosition.first, targetPosition.second, missileSpeed);
                missile.launch();
                federateAmbassador->activeMissiles.push_back(std::move(missile));
            }

            federateAmbassador->setFlagActiveMissile(*federateAmbassador, false);
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