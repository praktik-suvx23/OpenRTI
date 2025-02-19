/*
Implement the constructor and destructor.
Implement the runFederate method.
Implement methods for creating and connecting the RTI ambassador.
Implement methods for initializing the federation, joining the federation, and waiting for sync points.
Implement methods for initializing handles, subscribing to attributes, running the simulation loop, and resigning from the federation.
Implement methods for robot functionality (e.g., 
getPosition, 
getAltitude, 
getFuelLevel, 
getSpeed, 
split, 
toDegrees, 
reduceAltitude, 
calculateNewPosition, 
calculateInitialBearingDouble, 
calculateInitialBearingWstring, 
calculateDistance).
*/
#include "RobotFederate.h"

RobotFederate::RobotFederate() : gen(rd()), speedDis(0.0, 100.0) {
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
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, attributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

void RobotFederate::runSimulationLoop() {
    bool heightAchieved = false;
    federateAmbassador->currentPosition = getPosition(federateAmbassador->currentLatitude, federateAmbassador->currentLongitude);
    while (true) {
        federateAmbassador->currentSpeed = getSpeed(federateAmbassador->currentSpeed, 250.0, 450.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        federateAmbassador->currentFuelLevel = getFuelLevel(federateAmbassador->currentSpeed);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (!heightAchieved) {
            federateAmbassador->currentAltitude = getAltitude();
            if (federateAmbassador->currentAltitude >= 1000.0) {
                federateAmbassador->currentAltitude = 1000.0;
                heightAchieved = true;
            }
        }
        if (heightAchieved) {
            federateAmbassador->currentAltitude = reduceAltitude(federateAmbassador->currentAltitude, federateAmbassador->currentSpeed, federateAmbassador->currentDistance);
        }

        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
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

std::wstring RobotFederate::getPosition(double &currentLatitude, double &currentLongitude) {
    currentLatitude = 20.4382900;
    currentLongitude = 15.6253400;
    return std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
}

double RobotFederate::getAltitude() {
    static double altitude = 50.0;
    static bool increasing = true;
    static double angle = 60.0;

    if (increasing) {
        altitude += 50.0 * sin(angle * M_PI / 180);
        if (altitude > 1000.0) {
            altitude = 1000.0;
            increasing = false;
        }
    } else {
        altitude -= 50.0 * sin(angle * M_PI / 180);
        if (altitude < 0.0) {
            altitude = 0.0;
            increasing = true;
        }
    }
    return altitude;
}

double RobotFederate::getFuelLevel(double speed) {
    static double fuelLevel = 100.0;
    fuelLevel -= 0.01 * (speed / 100);
    if (fuelLevel < 0) {
        fuelLevel = 0;
    }
    return fuelLevel;
}

double RobotFederate::getSpeed(double cSpeed, double minSpeed, double maxSpeed) {
    speedDis.param(std::uniform_real_distribution<>::param_type(cSpeed - 10.0, cSpeed + 10.0));
    double newSpeed = speedDis(gen);
    if (newSpeed < minSpeed) {
        newSpeed = minSpeed;
    } else if (newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }
    return newSpeed;
}

std::vector<std::wstring> RobotFederate::split(const std::wstring &s, wchar_t delimiter) {
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

double RobotFederate::toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double RobotFederate::toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

double RobotFederate::reduceAltitude(double altitude, double speed, double distance) {
    double newAltitude = 0.0;
    if (altitude <= 200 && distance > 250) {
        newAltitude = 200;
    } else {
        newAltitude = (distance - speed * 0.1) * sin(asin(altitude / distance));
        if (newAltitude < 0) {
            newAltitude = 0;
        }
    }
    return newAltitude;
}

std::wstring RobotFederate::calculateNewPosition(const std::wstring &position, double speed, double bearing) {
    std::vector<std::wstring> tokens = split(position, L',');
    if (tokens.size() != 2) {
        return L"";
    }
    // Implementation of position calculation
    return L"";
}

double RobotFederate::calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2) {
    // Implementation of initial bearing calculation
    return 0.0;
}

double RobotFederate::calculateInitialBearingWstring(const std::wstring &position1, const std::wstring &position2) {
    std::vector<std::wstring> tokens1 = split(position1, L',');
    std::vector<std::wstring> tokens2 = split(position2, L',');
    if (tokens1.size() != 2 || tokens2.size() != 2) {
        return 0.0;
    }
    // Implementation of initial bearing calculation
    return 0.0;
}

double RobotFederate::calculateDistance(const std::wstring &position1, const std::wstring &position2, double currentAltitude) {
    std::vector<std::wstring> tokens1 = split(position1, L',');
    std::vector<std::wstring> tokens2 = split(position2, L',');
    if (tokens1.size() != 2 || tokens2.size() != 2) {
        return 0.0;
    }
    // Implementation of distance calculation
    return 0.0;
}

int main() {
    int numInstances = 1;

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
