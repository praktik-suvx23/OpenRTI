#include "RobotFederate.h"
#include "RobotFederateAmbassador.h"
#include "../include/Robot.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(250.0, 350.0);

RobotFederate::RobotFederate() {
    createRTIAmbassador();
}

RobotFederate::~RobotFederate() {
    resignFederation();
}

void startRobotSubscriber(int instance) {
    RobotFederate robotFederate;
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
        robotFederate.subscribeInteractions();
        robotFederate.publishInteractions();
        robotFederate.subscribeAttributes();
        robotFederate.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::createRTIAmbassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<MyFederateAmbassador>(rtiAmbassador.get());
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
        federateAmbassador->shipClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Position");
        federateAmbassador->attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FuturePosition");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FederateName");
        federateAmbassador->attributeHandleShipSize = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"ShipSize");
        federateAmbassador->attributeHandleNumberOfRobots = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"NumberOfRobots");
        federateAmbassador->attributeHandleShipLocked = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"isLockedBy");

        
        
        federateAmbassador->hitEventHandle = rtiAmbassador->getInteractionClassHandle(L"HitEvent");
        federateAmbassador->robotIDParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"RobotID");
        federateAmbassador->shipIDParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"ShipID");
        federateAmbassador->damageParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"DamageAmount");
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
        attributes.insert(federateAmbassador->attributeHandleShipLocked);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->hitEventHandle);
        std::wcout << L"Subscribed to HitEvent interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error subscribing to interactions: " << e.what() << std::endl;
    }
}

void RobotFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->hitEventHandle);
        std::wcout << L"Published HitEvent interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error publishing HitEvent: " << e.what() << std::endl;
    }
}

void RobotFederate::runSimulationLoop() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5500)); 
    bool heightAchieved = false;
    federateAmbassador->currentPosition = federateAmbassador->_robot.getPosition(federateAmbassador->currentLatitude, federateAmbassador->currentLongitude);

    // Store the previous position to calculate distance traveled
    std::wstring prevPosition = federateAmbassador->currentPosition;

    while (!federateAmbassador->getHitStatus()) {
        federateAmbassador->currentSpeed = federateAmbassador->_robot.getSpeed(federateAmbassador->currentSpeed, 250.0, 450.0);
        federateAmbassador->currentFuelLevel = federateAmbassador->_robot.getFuelLevel(federateAmbassador->currentSpeed);

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

        // Calculate the distance traveled
        double distanceTraveled = federateAmbassador->_robot.calculateDistance(prevPosition, federateAmbassador->currentPosition, federateAmbassador->currentAltitude);
        
        // Calculate the speed (distance / time)
        double timeElapsed = 0.1; // time elapsed between each update
        double robotSpeed = distanceTraveled / timeElapsed;

        // Print out the robot's speed
        std::wcout << L"Robot Speed: " << robotSpeed << L" m/s" << std::endl;

        prevPosition = federateAmbassador->currentPosition; // Update the previous position

        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
    sendHitEvent();
}

void RobotFederate::sendHitEvent() {
    try {
        rti1516e::ParameterHandleValueMap parameters;
        std::wcout << "[DEBUG] Sending HitEvent. FederateName: " << federateAmbassador->getFederateName() << ", ShipID: " << federateAmbassador->getTargetShipID() << std::endl;
        parameters[federateAmbassador->robotIDParam] = rti1516e::HLAunicodeString(federateAmbassador->getFederateName()).encode();
        parameters[federateAmbassador->shipIDParam] = rti1516e::HLAunicodeString(federateAmbassador->getTargetShipID()).encode();
        parameters[federateAmbassador->damageParam] = rti1516e::HLAinteger32BE(50).encode();

        rtiAmbassador->sendInteraction(federateAmbassador->hitEventHandle, parameters, rti1516e::VariableLengthData());
        std::wcout << L"Sent HitEvent!" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error sending HitEvent: " << e.what() << std::endl;
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
    int numInstances = 5;
    std::wofstream outFile("/usr/OjOpenRTI/OpenRTI/src/myProgram/log/finalData.txt", std::ios::trunc);
    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startRobotSubscriber, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}