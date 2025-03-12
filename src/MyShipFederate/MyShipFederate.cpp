#include "../include/shipHelperFunctions.h"
#include "../include/jsonParse.h"
#include "MyShipFederateAmbassador.h"
#include "MyShipFederate.h"

#include <RTI/encoding/BasicDataElements.h>
#include <thread>
#include <chrono>

MyShipFederate::MyShipFederate() {
    createRTIAmbassador();
}

MyShipFederate::~MyShipFederate() {
    resignFederation();
}

void startShipPublisher(int instance) {
    MyShipFederate myShip;
    myShip.federateAmbassador->setFederateName(L"ShipFederate " + std::to_wstring(instance));

    if(!myShip.rtiAmbassador) {
        std::wcerr << L"RTIambassador is null" << std::endl;
        exit(1);
    }

    try {

        myShip.readJsonFile(instance);
        myShip.connectToRTI();
        myShip.initializeFederation();
        myShip.joinFederation();
        myShip.waitForSyncPoint();
        myShip.initializeHandles();
        myShip.publishAttributes();
        myShip.registerShipObject();
        myShip.initializeTimeFactory();
        myShip.enableTimeManegement();
        myShip.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::readJsonFile(int i) {
    JsonParser parser(JSON_PARSER_PATH);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);
    if (!parser.isFileOpen()) return;
    if (i > 3) {
        i = dis(gen);
    }

    parser.parseShipConfig("Ship" + std::to_string(i));
    federateAmbassador->setshipNumber(L"Ship" + std::to_wstring(i));
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

void MyShipFederate::createRTIAmbassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MyShipFederateAmbassador>(rtiAmbassador.get());
}

void MyShipFederate::connectToRTI() {
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::initializeFederation() {
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

void MyShipFederate::joinFederation() {
    std::wstring federationName = L"robotFederation";
    try {
        rtiAmbassador->joinFederationExecution(federateAmbassador->getFederateName(), federationName);
        std::wcout << L"Federate: " << federateAmbassador->getFederateName() << L" - joined federation: " << federationName << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::waitForSyncPoint() {
    std::wcout << L"[DEBUG] federate: " << federateAmbassador->getFederateName() << L" waiting for sync point" << std::endl;
    try {
        // TODO - Add timeout
        while (federateAmbassador->getSyncLabel() != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::initializeHandles() {
    try {
        federateAmbassador->setObjectClassHandleShip(rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.Ship"));
        federateAmbassador->setAttributeHandleShipID(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipID"));
        federateAmbassador->setAttributeHandleShipTeam(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipTeam"));
        federateAmbassador->setAttributeHandleShipPosition(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Position"));
        federateAmbassador->setAttributeHandleShipSpeed(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"Speed"));
        federateAmbassador->setAttributeHandleShipSize(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"ShipSize"));
        federateAmbassador->setAttributeHandleNumberOfMissiles(rtiAmbassador->getAttributeHandle(federateAmbassador->getObjectClassHandleShip(), L"NumberOfMissiles"));
        std::wcout << L"Initialized attribute handles" << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::publishAttributes() {
    try {
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->getObjectClassHandleShip(), {
            federateAmbassador->getAttributeHandleShipID(),
            federateAmbassador->getAttributeHandleShipTeam(),
            federateAmbassador->getAttributeHandleShipPosition(),
            federateAmbassador->getAttributeHandleShipSpeed(),
            federateAmbassador->getAttributeHandleShipSize(),
            federateAmbassador->getAttributeHandleNumberOfMissiles(),
        });
        std::wcout << L"Published ship with attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::registerShipObject() {
    try {
        federateAmbassador->objectInstanceHandle = rtiAmbassador->registerObjectInstance(federateAmbassador->getMyObjectClassHandle());
        std::wcout << L"Registered ObjectInstance: " << federateAmbassador->objectInstanceHandle << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::updateShipAttributes(const std::wstring& shipLocation, 
    const std::wstring& futureShipLocation, double shipSpeed, const rti1516e::LogicalTime& logicalTimePtr) {
try {
    if (!federateAmbassador->objectInstanceHandle.isValid()) {
        std::wcerr << L"ERROR: Invalid RTI Ambassador or ObjectInstanceHandle!" << std::endl;
        return;
    }

    rti1516e::AttributeHandleValueMap attributes;
    attributes[federateAmbassador->getAttributeHandleShipID()] = rti1516e::HLAunicodeString(federateAmbassador->getFederateName()).encode();
    attributes[federateAmbassador->getAttributeHandleShipTeam()] = rti1516e::HLAunicodeString(L"Blue").encode();    // TODO: Change to something better
    attributes[federateAmbassador->getAttributeHandleShipPosition()] = rti1516e::HLAunicodeString(shipLocation).encode();
    attributes[federateAmbassador->getAttributeHandleShipSpeed()] = rti1516e::HLAfloat64BE(shipSpeed).encode();
    attributes[federateAmbassador->getAttributeHandleShipSize()] = rti1516e::HLAfloat64BE(federateAmbassador->getShipSize()).encode();
    attributes[federateAmbassador->getAttributeHandleNumberOfMissiles()] = rti1516e::HLAinteger32BE(federateAmbassador->getNumberOfRobots()).encode();

    rtiAmbassador->updateAttributeValues(
        federateAmbassador->objectInstanceHandle, 
        attributes, 
        rti1516e::VariableLengthData(),
        logicalTimePtr
    );

    std::wcout << L"Ship attributes updated successfully!" << std::endl;
} catch (const rti1516e::Exception& e) {
    std::wcerr << L"Error updating ship attributes: " << e.what() << std::endl;
}
}

void MyShipFederate::subscribeInteractions() { //TODO: Implement this
}

void MyShipFederate::initializeTimeFactory() {
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


void MyShipFederate::enableTimeManegement() {
    try {
        if (federateAmbassador->isRegulating) {  // Prevent enabling twice
            std::wcout << L"[WARNING] Time Regulation already enabled. Skipping..." << std::endl;
            return;
        }

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

void MyShipFederate::runSimulationLoop() {
    
    // Random number generation for ship speed 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(10.0, 25.0);
    std::uniform_real_distribution<> myDir(-5.0, 5.00);
    double publisherLat = 20.43829000; //for now check value in MyPublisher.cpp
    double publisherLon = 15.62534000; //for now check value in MyPublisher.cpp
    std::wstring myShipLocation = generateShipPosition(publisherLat, publisherLon);
    std::wstring futureExpectedPosition;
    bool firstPosition = true;  
    double currentDirection = myDir(gen); // Used for updateShipPosition function
    double currentSpeed = 0.0; // Used for updateShipPosition function
    double maxTurnRate = 5.0; // Maximum turn rate in degrees per tick

    double stepsize = 0.5;
    double simulationTime = 0.0;
    double maxSimulationTime = 100.0;

    try {
        while (simulationTime < maxSimulationTime) {
            // Logical time initialization

            if (!logicalTimeFactory) {
                std::wcerr << L"Logical time factory is null" << std::endl;
                exit(1);
            }

            rti1516e::HLAfloat64Time logicalTime(simulationTime + stepsize);
            //auto logicalTimePtr = logicalTimeFactory->makeLogicalTime(simulationTime + stepsize);

            // Update ship values, make this a function or integrate into updateShipAttributes
            currentSpeed = dis(gen);
            currentDirection = myDir(gen);
            currentDirection = getAngle(currentDirection, maxTurnRate);
            myShipLocation = updateShipPosition(myShipLocation, currentSpeed, currentDirection);
            futureExpectedPosition = updateShipPosition(myShipLocation, currentSpeed, currentDirection);

            updateShipAttributes(myShipLocation, futureExpectedPosition, currentSpeed, logicalTime);

            //For debugging
            std::wcout << L"Instance: " << federateAmbassador->getFederateName() << std::endl
                       << L"Current Position: " << myShipLocation << std::endl
                       << L"Future Position: " << futureExpectedPosition << std::endl
                       << L"Speed: " << currentSpeed << std::endl;
            //End debugging

            //LogicalTime
            federateAmbassador->isAdvancing = true;
            rtiAmbassador->timeAdvanceRequest(logicalTime);

            while (federateAmbassador->isAdvancing) {
                rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
            }
            std::wcout << L"[DEBUG] Main loop" << std::endl;
            simulationTime += stepsize;
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

// Might not be needed... or it should send that it's been hit to other ships

void MyShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 1; // Number of instances to start
    
    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startShipPublisher, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    return 0;
}