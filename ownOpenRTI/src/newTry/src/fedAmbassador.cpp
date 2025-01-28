#include "../include/fedAmbassador.h"
#include <iostream>
#include <cstring>

FederateAmbassador::FederateAmbassador() : _callbackState(0), _vehiclePosition(0.0), _vehicleSpeed(0.0) {
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
    std::cout << "Interaction received." << std::endl;
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
    for (const auto& attribute : attributeValues) {
        if (attribute.first == positionHandle) {
            std::memcpy(&currentPositionValue, attribute.second.data(), sizeof(currentPositionValue));
        } else if (attribute.first == speedHandle) {
            std::memcpy(&currentSpeedValue, attribute.second.data(), sizeof(currentSpeedValue));
        }
    }
    valuesUpdated = true;
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