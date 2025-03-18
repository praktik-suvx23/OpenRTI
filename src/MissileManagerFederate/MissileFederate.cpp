#include "MissileFederate.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

MissileFederate::MissileFederate(int instance) 
: gen(rd()), speedDis(250.0, 350.0) {
    createRTIAmbassador(instance);
}

MissileFederate::~MissileFederate() {
    resignFederation();
}

void startRobotSubscriber(int instance) {
    MissileFederate MissileFederate(instance);
    MissileFederate.federateAmbassador->setFederateName(L"MissileFederate " + std::to_wstring(instance));

    if(!MissileFederate.rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {
        MissileFederate.connectToRTI();
        MissileFederate.initializeFederation();
        MissileFederate.joinFederation();
        MissileFederate.waitForSyncPoint();
        MissileFederate.initializeHandles();
        MissileFederate.subscribeAttributes();
        MissileFederate.subscribeInteractions();
        MissileFederate.initializeTimeFactory();
        MissileFederate.enableTimeManagement();
        MissileFederate.runSimulationLoop();
    } 
    catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::createRTIAmbassador(int instance) {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MissileFederateAmbassador>(rtiAmbassador.get(), instance);
}

void MissileFederate::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::initializeFederation() {
    std::wstring federationName = L"robotFederation";
    std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
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

void MissileFederate::joinFederation() {
    std::wstring federationName = L"robotFederation";
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getFederateName() << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::waitForSyncPoint() {
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

void MissileFederate::initializeHandles() {
    try {
        //Adjust accordingly of the attributes you want to subscribe to
        federateAmbassador->setMyObjectClassHandle(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Position"));
        federateAmbassador->setAttributeHandleFutureShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FuturePosition"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"Speed"));
        federateAmbassador->setAttributeHandleFederateName(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"FederateName"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfRobots(rtiAmbassador->getAttributeHandle(federateAmbassador->getMyObjectClassHandle(), L"NumberOfMissiles"));
        std::wcout << L"Object handles initialized" << std::endl;

        federateAmbassador->setFireRobotHandle(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireRobot"));
        federateAmbassador->setFireRobotHandleParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Fire"));
        federateAmbassador->setTargetParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"Target"));
        federateAmbassador->setTargetPositionParam(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"TargetPosition"));
        federateAmbassador->setStartPosRobot(rtiAmbassador->getParameterHandle(federateAmbassador->getFireRobotHandle(), L"ShooterPosition"));
        
        //std::wcout << L"Interaction handles initialized" << std::endl;


    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MissileFederate::subscribeAttributes() {
    try {
        //Adjust accordingly of the attributes you want to subscribe to
        rti1516e::AttributeHandleSet attributes;
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

void MissileFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getFireRobotHandle());
        std::wcout << L"Subscribed to fire interaction" << std::endl;
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

void MissileFederate::runSimulationLoop() { //The main simulation loop

    
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

    //federateAmbassador->setCurrentPosition(federateAmbassador->_robot.getPosition(currentLatitude, currentLongitude));
    while (simulationTime < 100.0) { //Change this condition to hit when implemented, for now uses a timeout
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
                    std::ofstream outFile(DATA_LOG_PATH, std::ios::trunc);
                    if (outFile.is_open()) {
                        for (const auto& entry : finalData) {
                            outFile << std::string(entry.begin(), entry.end()) << std::endl;
                        }
                        outFile.close();
                        std::wcout << L"Data successfully written to finalData.txt" << std::endl;
                    } else {
                        std::wcerr << L"Unable to open file: finalData.txt" << std::endl;
                    }
                }
                std::wcout << L"Target reached" << std::endl;
                federateAmbassador->setCurrentDistance(federateAmbassador->_robot.calculateDistance(federateAmbassador->getCurrentPosition(), federateAmbassador->getShipPosition(), federateAmbassador->getCurrentAltitude()));
                std::wcout << L"Instance " << federateAmbassador->instance << L": Distance between robot and ship before last contact: " << federateAmbassador->getCurrentDistance() << " meters" << std::endl;
                rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
            }
        }
        simulationTime += stepsize; //Makes the simulation time advance
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