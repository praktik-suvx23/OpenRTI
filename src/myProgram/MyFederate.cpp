#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include "include/ObjectInstanceHandleHash.h" // Include the custom hash function

//function declarations
double toRadians(double degrees);
double calculateDistance(const std::wstring& position1, const std::wstring& position2, double currentAltitude);
double calculateInitialBearingWstring(const std::wstring& position1, const std::wstring& position2);
double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2);
std::wstring calculateNewPosition(const std::wstring& position, double speed, double bearing);
double reduceAltitude(double altitude, double speed, double distance);

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, const std::wstring& expectedPublisherName, const std::wstring& expectedShipName, int instance)
        : _rtiAmbassador(rtiAmbassador), _expectedPublisherName(expectedPublisherName), _expectedShipName(expectedShipName), _instance(instance) {}

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const& theObjectName) override {
        std::wcout << L"Instance " << _instance << L": Discovered ObjectInstance: " << theObject << L" of class: " << theObjectClass << std::endl;
        if (theObjectClass == objectClassHandle) {
            _objectInstances[theObject] = theObjectClass;
        } else if (theObjectClass == shipClassHandle) {
            _shipInstances[theObject] = theObjectClass;
        }
    }

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const& theAttributes,
        rti1516e::VariableLengthData const& theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::SupplementalReflectInfo theReflectInfo) override {
    
        auto itFederateName = theAttributes.find(attributeHandleFederateName);
        auto itShipFederateName = theAttributes.find(attributeHandleShipFederateName);
        if (itFederateName != theAttributes.end() || itShipFederateName != theAttributes.end()) {
            rti1516e::HLAunicodeString attributeValueFederateName;
            if (itFederateName != theAttributes.end()) {
                attributeValueFederateName.decode(itFederateName->second);
            } else if (itShipFederateName != theAttributes.end()) {
                attributeValueFederateName.decode(itShipFederateName->second);
            }
    
            if (attributeValueFederateName.get() != _expectedPublisherName && attributeValueFederateName.get() != _expectedShipName) {
                return;
            } else {
                std::wcout << L"Instance " << _instance << L": Update from federate: " << attributeValueFederateName.get() << std::endl << std::endl;
            }
        }
    
        if (_objectInstances.find(theObject) != _objectInstances.end()) {
            // Handle robot attributes
            auto itName = theAttributes.find(attributeHandleName);
            auto itSpeed = theAttributes.find(attributeHandleSpeed);
            auto itFuelLevel = theAttributes.find(attributeHandleFuelLevel);
            auto itPosition = theAttributes.end();
            if (firstPosition) {
                itPosition = theAttributes.find(attributeHandlePosition);
            }
            auto itAltitude = theAttributes.find(attributeHandleAltitude);
    
            if (itName != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueName;
                attributeValueName.decode(itName->second);
                std::wcout << L"Instance " << _instance << L": Received Name: " << attributeValueName.get() << std::endl;
            }
            if (itSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueSpeed;
                attributeValueSpeed.decode(itSpeed->second);
                std::wcout << L"Instance " << _instance << L": Received Speed: " << attributeValueSpeed.get() << " m/s"  << std::endl;
                currentSpeed = attributeValueSpeed.get();
            }
            if (itFuelLevel != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueFuelLevel;
                attributeValueFuelLevel.decode(itFuelLevel->second);
                std::wcout << L"Instance " << _instance << L": Received FuelLevel: " << attributeValueFuelLevel.get() << std::endl;
            }
            if (itPosition != theAttributes.end() && firstPosition) {
                rti1516e::HLAunicodeString attributeValuePosition;
                attributeValuePosition.decode(itPosition->second);
                std::wcout << L"Instance " << _instance << L": Received Position: " << attributeValuePosition.get() << std::endl;
                _currentPosition = attributeValuePosition.get();
    
                // Unsubscribe from position attribute after the first update
                try {
                    _rtiAmbassador->unsubscribeObjectClassAttributes(objectClassHandle, {attributeHandlePosition});
                    firstPosition = false;
                    std::wcout << L"Instance " << _instance << L": Unsubscribed from Position attribute" << std::endl;
                } catch (const rti1516e::Exception& e) {
                    std::wcerr << L"Instance " << _instance << L": Failed to unsubscribe from Position attribute: " << e.what() << std::endl;
                    return;
                }
            }
            
            if (itAltitude != theAttributes.end() && !heightAchieved) {
                rti1516e::HLAfloat64BE attributeValueAltitude;
                attributeValueAltitude.decode(itAltitude->second);
                std::wcout << L"Instance " << _instance << L": Received Altitude: " << attributeValueAltitude.get() << std::endl;
                currentAltitude = attributeValueAltitude.get();
                if (currentAltitude == 3000.0) {
                    std::wcout << L"Instance " << _instance << L": Altitude is 3000 meters" << std::endl;
                    try {
                        _rtiAmbassador->unsubscribeObjectClassAttributes(objectClassHandle, {attributeHandleAltitude});
                        heightAchieved = true;
                        std::wcout << L"Instance " << _instance << L": Unsubscribed from Altitude attribute" << std::endl;
                    } catch (const rti1516e::Exception& e) {
                        std::wcerr << L"Instance " << _instance << L": Failed to unsubscribe from Altitude attribute: " << e.what() << std::endl;
                    }
                    return;
                }
            }
        } else if (_shipInstances.find(theObject) != _shipInstances.end()) {
            // Handle ship attributes
            auto itShipTag = theAttributes.find(attributeHandleShipTag);
            auto itShipPosition = theAttributes.find(attributeHandleShipPosition);
            auto itFutureShipPosition = theAttributes.find(attributeHandleFutureShipPosition);
            auto itShipSpeed = theAttributes.find(attributeHandleShipSpeed);
    
            if (itShipTag != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipTag;
                attributeValueShipTag.decode(itShipTag->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Tag: " << attributeValueShipTag.get() << std::endl;
            }
            if (itShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueShipPosition;
                attributeValueShipPosition.decode(itShipPosition->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Position: " << attributeValueShipPosition.get() << std::endl;
                _shipPosition = attributeValueShipPosition.get();
            }
            if (itFutureShipPosition != theAttributes.end()) {
                rti1516e::HLAunicodeString attributeValueFutureShipPosition;
                attributeValueFutureShipPosition.decode(itFutureShipPosition->second);
                std::wcout << L"Instance " << _instance << L": Received Future Ship Position: " << attributeValueFutureShipPosition.get() << std::endl;
                _expectedShipPosition = attributeValueFutureShipPosition.get();
            }
            if (itShipSpeed != theAttributes.end()) {
                rti1516e::HLAfloat64BE attributeValueShipSpeed;
                attributeValueShipSpeed.decode(itShipSpeed->second);
                std::wcout << L"Instance " << _instance << L": Received Ship Speed: " << attributeValueShipSpeed.get() << std::endl;
            }
    
            // Calculate distance and initial bearing between publisher and ship positions
            try {
                if (!firstPosition) {
                    double initialBearing = calculateInitialBearingWstring(_currentPosition, _shipPosition);
                    _currentPosition = calculateNewPosition(_currentPosition, currentSpeed, initialBearing);
                    currentDistance = calculateDistance(_currentPosition, _shipPosition, currentAltitude);
                    currentAltitude = reduceAltitude(currentAltitude, currentSpeed, currentDistance);
                    _expectedFuturePosition = calculateNewPosition(_currentPosition, currentSpeed, initialBearing);
                    std::wcout << std::endl << L"Instance " << _instance << L": Robot Current Position: " << _currentPosition << std::endl;
                    std::wcout << L"Instance " << _instance << L": Ship Current Position: " << _shipPosition << std::endl;
                    std::wcout << L"Instance " << _instance << L": Robot Future Position: " << _expectedFuturePosition << std::endl;
                    std::wcout << L"Instance " << _instance << L": Ship Future Position: " << _expectedShipPosition << std::endl;
                    std::wcout << L"Instance " << _instance << L": Robot Current Altitude: " << currentAltitude << std::endl;
                }
                if (currentDistance < 50)
                    currentDistance = 10;
                std::wcout << L"Instance " << _instance << L": Distance between robot and ship: " << currentDistance << " meters" << std::endl;
                if (currentDistance < 1000) {
                    std::wcout << L"Instance " << _instance << L": Robot is within 1000 meters of target" << std::endl;
                    if (currentDistance < 100) {
                        std::wcout << L"Instance " << _instance << L": Robot is within 100 meters of target" << std::endl;
                        if (currentDistance < 50) {
                            std::wcout << L"Target reached" << std::endl;
                            currentDistance = calculateDistance(_currentPosition, _shipPosition, currentAltitude);
                            std::wcout << L"Instance " << _instance << L": Distance between robot and ship before last contact: " << currentDistance << " meters" << std::endl;
                            _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
                        }
                    }
                }
            } catch (const std::invalid_argument& e) {
                std::wcerr << L"Instance " << _instance << L": Invalid position format" << std::endl;
            }
        }
    }

    //MyRobot definitions
    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandleName;
    rti1516e::AttributeHandle attributeHandleSpeed;
    rti1516e::AttributeHandle attributeHandleFuelLevel;
    rti1516e::AttributeHandle attributeHandlePosition;
    rti1516e::AttributeHandle attributeHandleAltitude;
    rti1516e::AttributeHandle attributeHandleFederateName;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _objectInstances;

    //MyShip definitions
    rti1516e::ObjectClassHandle shipClassHandle;
    rti1516e::AttributeHandle attributeHandleShipTag;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleFutureShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    std::wstring _expectedPublisherName;
    std::wstring _expectedShipName;

    //HitEvent definitions
    rti1516e::InteractionClassHandle hitEventHandle;
    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle hitConfirmedParam;

    //Ex
    std::wstring _publisherPosition;
    std::wstring _shipPosition;

    bool firstPosition = true;
    bool heightAchieved = false;
    double currentSpeed;
    double currentAltitude;
    double currentDistance;

    std::wstring _currentPosition;
    std::wstring _expectedFuturePosition;
    std::wstring _expectedShipPosition;
    int _instance;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

std::vector<std::wstring> split(const std::wstring& s, wchar_t delimiter) {
    std::vector<std::wstring> tokens;
    std::wstring token;
    std::wstringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert degrees to radians
double toRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

double toDegrees(double radians) {
    return radians * 180.0 / M_PI;
}

double reduceAltitude(double altitude, double speed, double distance) {
    
    double newAltitude = (distance - speed * 0.1) * sin(asin(altitude / distance));
    if (newAltitude < 0) {
        newAltitude = 0;
    }

    return newAltitude;
}

std::wstring calculateNewPosition(const std::wstring& position, double speed, double bearing) {
    std::vector<std::wstring> tokens = split(position, L',');
    if (tokens.size() != 2) {
        throw std::invalid_argument("Invalid position format");
    }

    double lat = std::stod(tokens[0]);
    double lon = std::stod(tokens[1]);

    const double R = 6371000; // Radius of the Earth in meters

    double distance = speed * 0.1; // Distance traveled in meters (since speed is in m/s and time is 1 second) change according to time
    double bearingRad = toRadians(bearing); // Convert bearing to radians

    double latRad = toRadians(lat);
    double lonRad = toRadians(lon);

    double newLat = asin(sin(latRad) * cos(distance / R) + cos(latRad) * sin(distance / R) * cos(bearingRad));
    double newLon = lonRad + atan2(sin(bearingRad) * sin(distance / R) * cos(latRad), cos(distance / R) - sin(latRad) * sin(newLat));

    // Convert from radians to degrees
    newLat = toDegrees(newLat);
    newLon = toDegrees(newLon);

    std::wstringstream wss;
    wss << std::fixed << std::setprecision(8) << newLat << L"," << std::fixed << std::setprecision(8) << newLon;
    return wss.str();
}

double calculateInitialBearingDouble(double lat1, double lon1, double lat2, double lon2) {
    double dLon = toRadians(lon2 - lon1);
    double y = sin(dLon) * cos(toRadians(lat2));
    double x = cos(toRadians(lat1)) * sin(toRadians(lat2)) - sin(toRadians(lat1)) * cos(toRadians(lat2)) * cos(dLon);
    double initialBearing = atan2(y, x);
    return fmod(toDegrees(initialBearing) + 360, 360);
}

double calculateInitialBearingWstring(const std::wstring& position1, const std::wstring& position2) {
    std::vector<std::wstring> tokens1 = split(position1, L',');
    std::vector<std::wstring> tokens2 = split(position2, L',');

    if (tokens1.size() != 2 || tokens2.size() != 2) {
        throw std::invalid_argument("Invalid position format");
    }

    double lat1 = std::stod(tokens1[0]);
    double lon1 = std::stod(tokens1[1]);
    double lat2 = std::stod(tokens2[0]);
    double lon2 = std::stod(tokens2[1]);

    return calculateInitialBearingDouble(lat1, lon1, lat2, lon2);
}

// Function to calculate distance between two positions given as wstrings
// Input value format: latitude,longitude (e.g., L"20.43829,15.62534")
double calculateDistance(const std::wstring& position1, const std::wstring& position2, double currentAltitude) {
    std::vector<std::wstring> tokens1 = split(position1, L','); //Used to split the latitude and longitude of position 1
    std::vector<std::wstring> tokens2 = split(position2, L','); //Used to split the latitude and longitude of position 2

    if (tokens1.size() != 2 || tokens2.size() != 2) {
        throw std::invalid_argument("Invalid position format");
    }

    double lat1 = std::stod(tokens1[0]);
    double lon1 = std::stod(tokens1[1]);
    double lat2 = std::stod(tokens2[0]);
    double lon2 = std::stod(tokens2[1]);

    const double R = 6371000; // Radius of the Earth in meters (average value)
    double dLat = toRadians(lat2 - lat1); //Delta latitude
    double dLon = toRadians(lon2 - lon1); //Delta longitude
    double haversineFormulaComponent = sin(dLat / 2) * sin(dLat / 2) +
                                       cos(toRadians(lat1)) * cos(toRadians(lat2)) *
                                       sin(dLon / 2) * sin(dLon / 2);
    double centralAngle = 2 * atan2(sqrt(haversineFormulaComponent), sqrt(1 - haversineFormulaComponent));
    double distance = R * centralAngle;
    distance = sqrt(pow(distance, 2) + pow(currentAltitude, 2));
    return distance; // In meters
}

void startSubscriber(int instance) {
    std::wstring federateName = L"Subscriber" + std::to_wstring(instance);
    std::wstring expectedPublisherName = L"Publisher" + std::to_wstring(instance);  // Expected publisher name
    std::wstring expectedShipName = L"ShipPublisher" + std::to_wstring(instance);   // Expected ship name

    try {
        // Create RTIambassador and connect with synchronous callback model
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        auto federateAmbassador = std::make_shared<MyFederateAmbassador>(rtiAmbassador.get(), expectedPublisherName, expectedShipName, instance);
        std::wcout << L"Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");    //Using the rti protocol, can be switched with other protocols

        // Create or join federation

        std::wstring federationName = L"robotFederation";
        std::vector<std::wstring> fomModules = {    // If you want to use more than one FOM module, add them to the vector
            L"foms/robot.xml"
      
        };
        std::wstring mimModule = L"foms/MIM.xml";
        try {
            rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
            std::wcout << L"Federation created: " << federationName << std::endl;
        } catch (const rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists: " << federationName << std::endl;
        }
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"MyFederate joined: " << federateName << std::endl;

        // Achieve sync point
        std::wcout << L"MyFederate waiting for synchronization announcement..." << std::endl;
        while(federateAmbassador->syncLabel != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"MyFederate received sync point." << std::endl;

        // Get handles and subscribe to object class attributes
        federateAmbassador->objectClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.robot");
        federateAmbassador->attributeHandleName = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"robot-x");
        federateAmbassador->attributeHandleSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Speed");
        federateAmbassador->attributeHandleFuelLevel = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FuelLevel");
        federateAmbassador->attributeHandlePosition = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Position");
        federateAmbassador->attributeHandleAltitude = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"Altitude");
        federateAmbassador->attributeHandleFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->objectClassHandle, L"FederateName");

        federateAmbassador->shipClassHandle = rtiAmbassador->getObjectClassHandle(L"HLAobjectRoot.ship");
        federateAmbassador->attributeHandleShipTag = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Ship-tag");
        federateAmbassador->attributeHandleShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Position");
        federateAmbassador->attributeHandleFutureShipPosition = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FuturePosition");
        federateAmbassador->attributeHandleShipSpeed = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"Speed");
        federateAmbassador->attributeHandleShipFederateName = rtiAmbassador->getAttributeHandle(federateAmbassador->shipClassHandle, L"FederateName");

        rti1516e::AttributeHandleSet robotAttributes;
        robotAttributes.insert(federateAmbassador->attributeHandleName);
        robotAttributes.insert(federateAmbassador->attributeHandleSpeed);
        robotAttributes.insert(federateAmbassador->attributeHandleFuelLevel);
        robotAttributes.insert(federateAmbassador->attributeHandlePosition);
        robotAttributes.insert(federateAmbassador->attributeHandleAltitude);
        robotAttributes.insert(federateAmbassador->attributeHandleFederateName);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->objectClassHandle, robotAttributes);
        std::wcout << L"Subscribed to robot attributes" << std::endl;

        rti1516e::AttributeHandleSet shipAttributes;
        shipAttributes.insert(federateAmbassador->attributeHandleShipTag);
        shipAttributes.insert(federateAmbassador->attributeHandleShipPosition);
        shipAttributes.insert(federateAmbassador->attributeHandleFutureShipPosition);
        shipAttributes.insert(federateAmbassador->attributeHandleShipSpeed);
        shipAttributes.insert(federateAmbassador->attributeHandleShipFederateName);
        rtiAmbassador->subscribeObjectClassAttributes(federateAmbassador->shipClassHandle, shipAttributes);
        std::wcout << L"Subscribed to ship attributes" << std::endl;

        // Main loop to process callbacks
        while (true) {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0); 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int numInstances = 100; // Number of instances of unique subscribers to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startSubscriber, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}