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
#include <random>
#include <cmath>
#include <iomanip>
#include "include/ObjectInstanceHandleHash.h" // Include the custom hash function
// Function declarations
std::wstring getPosition();
double getAltitude();
double getFuelLevel(double speed);
double getSpeed();

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
    
        if (_shipInstances.find(theObject) != _shipInstances.end()) {
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

    std::wstring syncLabel = L"";
    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag)
    {
        if (label == L"InitialSync") {
            std::wcout << L"Publisher Federate received synchronization announcement: InitialSync." << std::endl;
            syncLabel = label;
        }
    
        // Not in use
        if (label == L"ShutdownSync") {
            std::wcout << L"Publisher Federate received synchronization announcement: ShutdownSync." << std::endl;
            syncLabel = label; 
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
    // Random number generator for simulating sensor data
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> speedDis(0.0, 100.0); // Speed range: 0 to 100 units

std::wstring getPosition(double& currentLatitude, double& currentLongitude) {
    // Simulate position sensor reading (latitude, longitude)
    currentLatitude = 20.4382900; // Increment latitude
    currentLongitude = 15.6253400; // Increment longitude
    return std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
}

double getAltitude() {
    // Simulate altitude sensor reading with oscillation
    static double altitude = 50.0;
    static bool increasing = true;
    static double angle = 60.0; // Angle in degrees

    if (increasing) {
        altitude += 50.0 * sin(angle * M_PI / 180); // Increase altitude
        if (altitude > 3000.0) {
            altitude = 3000.0;
            increasing = false;
        }
    } 
    return altitude;
}

double getFuelLevel(double speed) {
    // Simulate fuel level sensor reading
    static double fuelLevel = 100.0;
    fuelLevel -= 0.1 * (speed/100); // Decrease fuel level
    if (fuelLevel < 0) {
        fuelLevel = 0;
    }
    return fuelLevel;
}

double getSpeed(double cSpeed, double minSpeed, double maxSpeed) {
    // Update the range of the speed distribution based on the current speed
    speedDis.param(std::uniform_real_distribution<>::param_type(cSpeed - 10.0, cSpeed + 10.0));
    // Generate a new random speed value within the updated range
    double newSpeed = speedDis(gen);
    // Clamp the speed value to ensure it stays within the specified range
    if (newSpeed < minSpeed) {
        newSpeed = minSpeed;
    } else if (newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }
    return newSpeed;
}
void startRobot(int instance){
    std::wstring federateName = L"Robot" + std::to_wstring(instance);
    std::wstring expectedShipName = L"Ship" + std::to_wstring(instance);

    try {
        auto rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    }
    catch (const rti1516e::Exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
}

int main() {
    int numInstances = 10; // Number of instances to start

    std::vector<std::thread> threads;
    for (int i = 1; i <= numInstances; ++i) {
        threads.emplace_back(startRobot, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    for (auto& thread : threads) {
        thread.join();
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
    }

    return 0;
}
