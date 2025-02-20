#include "RobotFederate.h"
#include "../include/Robot.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

RobotFederate::RobotFederate(int instance) : gen(rd()), speedDis(250.0, 350.0) {
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
        robotFederate.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
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
        federateAmbassador->shipClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Position");
        federateAmbassador->attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FuturePosition");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FederateName");
        federateAmbassador->attributeHandleShipSize = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"ShipSize");
        federateAmbassador->attributeHandleNumberOfRobots = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"NumberOfRobots");

    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::subscribeAttributes() {
    try {
        rti1516e::AttributeHandleSet attributes;
        attributes.insert(federateAmbassador->attributeHandleShipTag);
        attributes.insert(federateAmbassador->attributeHandleShipPosition);
        attributes.insert(federateAmbassador->attributeHandleFutureShipPosition);
        attributes.insert(federateAmbassador->attributeHandleShipSpeed);
        attributes.insert(federateAmbassador->attributeHandleShipFederateName);
        attributes.insert(federateAmbassador->attributeHandleShipSize);
        attributes.insert(federateAmbassador->attributeHandleNumberOfRobots);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::runSimulationLoop() {
    bool heightAchieved = false;
    federateAmbassador->currentPosition = federateAmbassador->_robot.getPosition(federateAmbassador->currentLatitude, federateAmbassador->currentLongitude);
    while (true) {
        federateAmbassador->currentSpeed = federateAmbassador->_robot.getSpeed(federateAmbassador->currentSpeed, 250.0, 450.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        federateAmbassador->currentFuelLevel = federateAmbassador->_robot.getFuelLevel(federateAmbassador->currentSpeed);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (!heightAchieved) {
            federateAmbassador->currentAltitude = federateAmbassador->_robot.getAltitude();
            if (federateAmbassador->currentAltitude >= 1000.0) {
                federateAmbassador->currentAltitude = 1000.0;
                heightAchieved = true;
            }
        }
        if (heightAchieved) {
            federateAmbassador->currentAltitude = federateAmbassador->_robot.reduceAltitude(federateAmbassador->currentAltitude, federateAmbassador->currentSpeed, federateAmbassador->currentDistance);
        }

        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        federateAmbassador->simulationTime += 0.1;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
    int numInstances = 3;
    std::wofstream outFile("/usr/OjOpenRTI/OpenRTI/src/myProgram/log/finalData.txt", std::ios::trunc);
    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startRobotSubscriber, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    return 0;
}