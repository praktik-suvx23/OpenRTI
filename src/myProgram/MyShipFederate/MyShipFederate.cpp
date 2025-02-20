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
        myShip.runSimulationLoop();
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::readJsonFile(int i) {
    JsonParser parser("/usr/OjOpenRTI/OpenRTI/src/myProgram/ShipData/ShipData.json");
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1, 3);
    if (!parser.isFileOpen()) return;
    if (i > 3) {
        i = dis(gen);
    }

    parser.parseShipConfig("Ship" + std::to_string(i));
    federateAmbassador->shipNumber = "Ship" + std::to_string(i);
    federateAmbassador->shiplength = parser.getLength();
    federateAmbassador->shipwidth = parser.getWidth();
    federateAmbassador->shipheight = parser.getHeight();
    federateAmbassador->ShipSize = federateAmbassador->shiplength * federateAmbassador->shipwidth * federateAmbassador->shipheight;
    federateAmbassador->numberOfRobots = parser.getNumberOfRobots();
    std::cout << L"Ship Number: " << federateAmbassador->shipNumber << std::endl;
    std::wcout << L"Ship Length: " << federateAmbassador->shiplength << std::endl;
    std::wcout << L"Ship Width: " << federateAmbassador->shipwidth << std::endl;
    std::wcout << L"Ship Height: " << federateAmbassador->shipheight << std::endl;
    std::wcout << L"Ship Size: " << federateAmbassador->ShipSize << std::endl;
    std::wcout << L"Number of Robots: " << federateAmbassador->numberOfRobots << std::endl;
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
        federateAmbassador->objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Position");
        federateAmbassador->attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FuturePosition");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FederateName");
        federateAmbassador->attributeHandleShipSize = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"ShipSize");
        federateAmbassador->attributeHandleNumberOfRobots = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"NumberOfRobots");

        federateAmbassador->hitEventHandle = rtiAmbassador->getInteractionClassHandle(L"HitEvent");
        federateAmbassador->robotIDParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"RobotID");
        federateAmbassador->shipIDParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"ShipID");
        federateAmbassador->damageParam = rtiAmbassador->getParameterHandle(federateAmbassador->hitEventHandle, L"DamageAmount");
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::publishAttributes() {
    try {
        rtiAmbassador->publishObjectClassAttributes(federateAmbassador->objectClassHandle, {
            federateAmbassador->attributeHandleShipTag,
            federateAmbassador->attributeHandleShipPosition,
            federateAmbassador->attributeHandleFutureShipPosition,
            federateAmbassador->attributeHandleShipSpeed,
            federateAmbassador->attributeHandleShipFederateName,
            federateAmbassador->attributeHandleShipSize,
            federateAmbassador->attributeHandleNumberOfRobots
        });
        std::wcout << L"Published ship with attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::registerShipObject() {
    try {
        federateAmbassador->objectInstanceHandle = rtiAmbassador->registerObjectInstance(federateAmbassador->objectClassHandle);
        std::wcout << L"Registered ObjectInstance: " << federateAmbassador->objectInstanceHandle << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void MyShipFederate::updateShipAttributes(const std::wstring& shipLocation, 
    const std::wstring& futureShipLocation, double shipSpeed) {
try {
    if (!federateAmbassador->objectInstanceHandle.isValid()) {
        std::wcerr << L"ERROR: Invalid RTI Ambassador or ObjectInstanceHandle!" << std::endl;
        return;
    }

    rti1516e::AttributeHandleValueMap attributes;
    attributes[federateAmbassador->attributeHandleShipFederateName] = rti1516e::HLAunicodeString(federateAmbassador->getFederateName()).encode();
    attributes[federateAmbassador->attributeHandleShipPosition] = rti1516e::HLAunicodeString(shipLocation).encode();
    attributes[federateAmbassador->attributeHandleFutureShipPosition] = rti1516e::HLAunicodeString(futureShipLocation).encode();
    attributes[federateAmbassador->attributeHandleShipSpeed] = rti1516e::HLAfloat64BE(shipSpeed).encode();
    attributes[federateAmbassador->attributeHandleShipSize] = rti1516e::HLAfloat64BE(federateAmbassador->ShipSize).encode();
    attributes[federateAmbassador->attributeHandleNumberOfRobots] = rti1516e::HLAinteger32BE(federateAmbassador->numberOfRobots).encode();

    rtiAmbassador->updateAttributeValues(federateAmbassador->objectInstanceHandle, attributes, rti1516e::VariableLengthData());

    std::wcout << L"Ship attributes updated successfully!" << std::endl;
} catch (const rti1516e::Exception& e) {
    std::wcerr << L"Error updating ship attributes: " << e.what() << std::endl;
}
}

void MyShipFederate::subscribeInteractions() {
    try {
        rtiAmbassador->subscribeInteractionClass(federateAmbassador->hitEventHandle);
        std::wcout << L"Subscribed to HitEvent interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error subscribing to interactions: " << e.what() << std::endl;
    }
}

void MyShipFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(federateAmbassador->hitEventHandle);
        std::wcout << L"Published HitEvent interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error publishing HitEvent: " << e.what() << std::endl;
    }
}

void MyShipFederate::runSimulationLoop() {
    // TODO: Temporary random values for testing

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

    try {
        while (federateAmbassador->getHitStatus() == false) {
            currentSpeed = dis(gen);
            currentDirection = myDir(gen);
            currentDirection = getAngle(currentDirection, maxTurnRate);
            myShipLocation = updateShipPosition(myShipLocation, currentSpeed, currentDirection);
            futureExpectedPosition = updateShipPosition(myShipLocation, currentSpeed, currentDirection);

            //for debugging
            std::wcout << L"Instance: " << federateAmbassador->getFederateName() 
                       << L", Current Position: " << myShipLocation 
                       << L", Future Position: " << futureExpectedPosition 
                       << L", Speed: " << currentSpeed << std::endl;
            //end debugging
            updateShipAttributes(myShipLocation, futureExpectedPosition, currentSpeed);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
    sendHitEvent();
}

void MyShipFederate::sendHitEvent() {
    try {
        rti1516e::ParameterHandleValueMap parameters;

        parameters[federateAmbassador->robotIDParam] = rti1516e::HLAunicodeString(federateAmbassador->getRobotID()).encode();
        parameters[federateAmbassador->shipIDParam] = rti1516e::HLAunicodeString(federateAmbassador->getFederateName()).encode(); 
        parameters[federateAmbassador->damageParam] = rti1516e::HLAinteger32BE(federateAmbassador->getDamageAmount()).encode();

        rtiAmbassador->sendInteraction(federateAmbassador->hitEventHandle, parameters, rti1516e::VariableLengthData());
        std::wcout << L"🚀 Sent HitEvent!" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Error sending HitEvent: " << e.what() << std::endl;
    }
}

void MyShipFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 100; // Number of instances to start
    
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