#include "MissileFederate.h"
#include "../include/MissileCalculator.h"

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
    initializeFederation();
    joinFederation();
    waitForSyncPoint();
    initializeHandles();
    subscribeAttributes();
    subscribeInteractions();
    waitForSetupSync();
    initializeTimeFactory();
    enableTimeManagement();
    runSimulationLoop();
}

void MissileFederate::createRTIAmbassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MissileFederateAmbassador>(rtiAmbassador.get());
}

void MissileFederate::connectToRTI() {
    try {
        if(!rtiAmbassador) {
            std::wcerr << L"RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"Federate: " << federateName << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::waitForSyncPoint() {
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

void MissileFederate::initializeHandles() {
    try {
        // For setup object class ship and its attributes. Subscribe
        federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
        federateAmbassador->setAttributeHandleFutureShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"FuturePosition"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));

        // For setup object class missile and its attributes. Publish
        federateAmbassador->setObjectClassHandleMissile(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Missile"));
        federateAmbassador->setAttributeHandleMissileID(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileID"));
        federateAmbassador->setAttributeHandleMissileTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"MissileTeam"));
        federateAmbassador->setAttributeHandleMissilePosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Position"));
        federateAmbassador->setAttributeHandleMissileAltitude(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Altitude"));
        federateAmbassador->setAttributeHandleMissileSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleMissile(), L"Speed"));

        // Remove this when ready to replace with FireMissile interaction
        federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
        federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
        federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
        federateAmbassador->setTargetPositionParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"TargetPosition"));
        federateAmbassador->setStartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));

        // For setup interaction class StartSimulation and its parameters. Subscribe
        federateAmbassador->setInteractionClassSetupSimulation(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
        federateAmbassador->setParamTimeScaleFactor(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassSetupSimulation(), L"TimeScaleFactor"));

        // For setup interaction class FireMissile and its parameters. Subscribe
        federateAmbassador->setInteractionClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
        federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterID"));
        federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"Team"));
        federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"ShooterPosition"));
        federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"TargetPosition"));
        federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassFireMissile(), L"NumberOfMissilesFired"));

        // For setup interaction class MissileFlight and its parameters. Publish and subscribe on 'MissileDestroyed' in case of Missile destruction before reaching target
        federateAmbassador->setInteractionClassMissileFlight(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.MissileFlight"));
        federateAmbassador->setParamMissileFlightID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileID"));
        federateAmbassador->setParamMissileFlightTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileTeam"));
        federateAmbassador->setParamMissileFlightPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissilePosition"));
        federateAmbassador->setParamMissileFlightAltitude(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileAltitude"));
        federateAmbassador->setParamMissileFlightSpeed(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileSpeed"));
        federateAmbassador->setParamMissileFlightLockOnTargetID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileLockOnTargetID"));
        federateAmbassador->setParamMissileFlightHitTarget(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileHitTarget"));
        federateAmbassador->setParamMissileFlightDestroyed(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractionClassMissileFlight(), L"MissileDestroyed"));

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
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
        std::wcerr << L"Exception: " << e.what() << std::endl;
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
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::subscribeInteractions() {
    try {
        // Subscribe to interaction classes
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getFireRobotHandle());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getFireRobotHandle()) 
                   << std::endl;

        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassSetupSimulation());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassSetupSimulation()) 
                   << std::endl;

        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassFireMissile());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassFireMissile()) 
                   << std::endl;

        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractionClassMissileFlight());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassMissileFlight()) 
                   << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->getFireRobotHandle()); // Remove me when ready
        std::wcout << L"Published interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getFireRobotHandle()) 
                   << std::endl;
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractionClassMissileFlight());
        std::wcout << L"Published interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractionClassMissileFlight()) 
                   << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::waitForSetupSync() {
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
        auto lookahead = rti1516e::HLAfloat64Interval(0.5);  // Lookahead must be > 0
        std::wcout << L"[INFO] Enabling Time Management..." << std::endl;
        
        rtiAmbassador->enableTimeRegulation(lookahead);
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

void MissileFederate::runSimulationLoop() {
    bool hitTarget = false;
    bool heightAchieved = false;
    double stepsize = 0.5;
    double simulationTime = 0.0;

    // Ensure logical time is initialized correctly
    if (!logicalTimeFactory) {
        std::wcerr << L"Logical time factory is null" << std::endl;
        exit(1);
    }
    
    rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
    
    federateAmbassador->setIsAdvancing(true);
    rtiAmbassador->timeAdvanceRequest(logicalTime);

    std::wcout << L"[DEBUG] Waiting for time advance" << std::endl;
    while (federateAmbassador->getIsAdvancing()) {
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    std::wcout << L"[DEBUG] Starting simulation loop" << std::endl;
    while (!hitTarget) {
        while (!federateAmbassador->getCreateNewMissile()) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        federateAmbassador->setCreateNewMissile(false);
        federateAmbassador->setStartTime(std::chrono::high_resolution_clock::now());
        
        std::pair<double, double> missileStartPosition = federateAmbassador->getMissilePosition();
        std::pair<double, double> missileTargetPosition = federateAmbassador->getMissileTargetPosition();

        double initialBearing = calculateInitialBearingDouble(missileStartPosition.first, missileStartPosition.second, missileTargetPosition.first, missileTargetPosition.second);

        while (!hitTarget) {
            federateAmbassador->setCurrentSpeed(getSpeed(federateAmbassador->getCurrentSpeed(), 250.0, 450.0));

            if (!heightAchieved) {
                federateAmbassador->setCurrentAltitude(getAltitude());
                if (federateAmbassador->getCurrentAltitude() >= 1000.0) {
                    federateAmbassador->setCurrentAltitude(1000.0);
                    heightAchieved = true;
                }
            } else {
                federateAmbassador->setCurrentAltitude(reduceAltitude(
                    federateAmbassador->getCurrentAltitude(), 
                    federateAmbassador->getCurrentSpeed(), 
                    federateAmbassador->getCurrentDistance()
                ));
            }

            // Update logical time before advancing
            logicalTime = rti1516e::HLAfloat64Time(simulationTime + stepsize);
            federateAmbassador->setIsAdvancing(true);
            rtiAmbassador->timeAdvanceRequest(logicalTime);

            while (federateAmbassador->getIsAdvancing()) {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }

            federateAmbassador->setMissilePosition(calculateNewPosition(federateAmbassador->getMissilePosition(), federateAmbassador->getCurrentSpeed(), initialBearing));
            federateAmbassador->setCurrentDistance(calculateDistance(federateAmbassador->getMissilePosition(), federateAmbassador->getMissileTargetPosition(), federateAmbassador->getCurrentAltitude()));

            std::wcout << L"[INFO] Missile Current Position: " << federateAmbassador->getMissilePosition().first << ", " << federateAmbassador->getMissilePosition().second << std::endl;
            std::wcout << L"[INFO] Distance between missile and target: " << federateAmbassador->getCurrentDistance() << " meters" << std::endl;

            if (federateAmbassador->getCurrentDistance() < 50) {
                hitTarget = true;
                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> realTimeDuration = endTime - federateAmbassador->getStartTime();
                double realTime = realTimeDuration.count();
                const auto& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(logicalTime);
                double federateSimulationTime = floatTime.getTime();

                std::vector<std::wstring> finalData = {
                    L"--------------------------------------------",
                    L"Instance : ~tempDonkey~",
                    L"Last Distance : " + std::to_wstring(federateAmbassador->getCurrentDistance()) + L" meters",
                    L"Last Altitude : " + std::to_wstring(federateAmbassador->getCurrentAltitude()) + L" meters",
                    L"Last Speed : " + std::to_wstring(federateAmbassador->getCurrentSpeed()) + L" m/s",
                    L"Last position for missile : " + 
                        std::to_wstring(federateAmbassador->getMissilePosition().first) + 
                        L", " + 
                        std::to_wstring(federateAmbassador->getMissilePosition().second),
                    L"Last position for ship : " + 
                        std::to_wstring(federateAmbassador->getMissileTargetPosition().first) + 
                        L", " + 
                        std::to_wstring(federateAmbassador->getMissileTargetPosition().second),
                    L"Simulation time : " + std::to_wstring(federateSimulationTime) + L" seconds",
                    L"Real time : " + std::to_wstring(realTime) + L" seconds",
                    L"--------------------------------------------"
                };

                std::ofstream outFile(DATA_LOG_PATH, std::ios::app);
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
                std::wcout << L"Distance before last contact: " << federateAmbassador->getCurrentDistance() << " meters" << std::endl;
            }

            simulationTime += stepsize;
        }
    }
}


void MissileFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::wofstream outFile(DATA_LOG_PATH, std::ios::trunc); //See Data_LOG_PATH in CMakeLists.txt

    try {
        MissileFederate missileManagerFederate;
        missileManagerFederate.startMissileManager(); // Call the member function
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}