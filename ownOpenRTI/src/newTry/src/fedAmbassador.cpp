#include "../include/fedAmbassador.h"
#include <iostream>
#include <cstring>

FederateAmbassador::FederateAmbassador() 
    : _callbackState(0), _vehiclePosition(0.0), _vehicleSpeed(0.0), lastUpdateTime(std::chrono::steady_clock::now()) {
}

FederateAmbassador::~FederateAmbassador() {
}

void FederateAmbassador::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                                            rti1516e::ParameterHandleValueMap const &theParameters,
                                            rti1516e::VariableLengthData const &theUserSuppliedTag,
                                            rti1516e::OrderType sentOrder,
                                            rti1516e::TransportationType theType,
                                            rti1516e::LogicalTime const &theTime,
                                            rti1516e::OrderType receivedOrder,
                                            rti1516e::MessageRetractionHandle theHandle,
                                            rti1516e::SupplementalReceiveInfo theReceiveInfo) {
    std::wcout << "Interaction received." << std::endl;
}

void FederateAmbassador::reflectAttributeValues(rti1516e::ObjectInstanceHandle objectInstanceHandle,
                                                const rti1516e::AttributeHandleValueMap& attributeValues,
                                                const rti1516e::VariableLengthData& tag,
                                                rti1516e::OrderType sentOrder,
                                                rti1516e::TransportationType transportationType,
                                                const rti1516e::LogicalTime& time,
                                                rti1516e::OrderType receivedOrder,
                                                rti1516e::MessageRetractionHandle retractionHandle,
                                                rti1516e::SupplementalReflectInfo reflectInfo) {
    std::unique_lock<std::mutex> lock(mutex);
    std::wstring wstr = objectInstanceHandle.toString();
    std::string str(wstr.begin(), wstr.end()); // Convert wstring to string
    std::cout << "Message from fedAmbassador.cpp: Received ObjectInstanceHandle: " << str << std::endl;
    for (const auto& attribute : attributeValues) {
        if (attribute.first == positionHandle) {
            std::memcpy(&currentPositionValue, attribute.second.data(), sizeof(currentPositionValue));
            _vehiclePosition = currentPositionValue; // Update vehicle position
        } else if (attribute.first == speedHandle) {
            std::memcpy(&currentSpeedValue, attribute.second.data(), sizeof(currentSpeedValue));
            _vehicleSpeed = currentSpeedValue; // Update vehicle speed
        }
    }

    valuesUpdated = true;
    lastUpdateTime = std::chrono::steady_clock::now(); // Update last update time
    cv.notify_all();
}

void FederateAmbassador::setVehiclePosition(double position) {
    std::unique_lock<std::mutex> lock(mutex);
    _vehiclePosition = position;
    notifyAttributeUpdate();
}

double FederateAmbassador::getVehiclePosition() const {
    std::unique_lock<std::mutex> lock(mutex);
    return _vehiclePosition;
}

void FederateAmbassador::setVehicleSpeed(double speed) {
    std::unique_lock<std::mutex> lock(mutex);
    _vehicleSpeed = speed;
    notifyAttributeUpdate();
}

double FederateAmbassador::getVehicleSpeed() const {
    std::unique_lock<std::mutex> lock(mutex);
    return _vehicleSpeed;
}

void FederateAmbassador::notifyAttributeUpdate() {
    // Notify other components or systems about the attribute update
    valuesUpdated = true;
    cv.notify_all();
}

void FederateAmbassador::waitForUpdate() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return valuesUpdated; });
    valuesUpdated = false; // Reset the flag after processing the update
}

void FederateAmbassador::receiveAttributeValue(const rti1516e::ObjectInstanceHandle& objectInstanceHandle,
                                     const rti1516e::AttributeHandleSet& attributes,
                                     const rti1516e::LogicalTime& time) {
    if (attributes.find(positionHandle) != attributes.end()) {
        std::cout << "Position updated." << std::endl;
        // Update your flag or process the position data
        receivedNewData = true; // Set the flag if new data is received
    }
    if (attributes.find(speedHandle) != attributes.end()) {
        std::cout << "Speed updated." << std::endl;
        // Update your flag or process the speed data
        receivedNewData = true; // Set the flag if new data is received
    }
}

// This method will be used by the main Federate to check if an update has been received within 15 seconds
bool FederateAmbassador::isUpdateTimeout() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdateTime).count();
    return elapsed > 15; // Returns true if more than 15 seconds have passed since the last update
}
