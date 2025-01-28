#include "MyFederateAmbassador.h"
#include <iostream>

MyFederateAmbassador::MyFederateAmbassador()
    : currentPositionValue(0.0), currentSpeedValue(0.0), valuesUpdated(false) {}

void MyFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle objectInstanceHandle,
    const rti1516e::AttributeHandleValueMap& attributeValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    const rti1516e::LogicalTime& time,
    rti1516e::OrderType receivedOrder,
    rti1516e::MessageRetractionHandle retractionHandle,
    rti1516e::SupplementalReflectInfo reflectInfo) {
    std::unique_lock<std::mutex> lock(mutex);
    std::cout << "reflectAttributeValues callback invoked" << std::endl;
    for (const auto& attribute : attributeValues) {
        if (attribute.first == positionHandle) {
            std::memcpy(&currentPositionValue, attribute.second.data(), sizeof(currentPositionValue));
            std::cout << "Received Position Value: " << currentPositionValue << std::endl;
        } else if (attribute.first == speedHandle) {
            std::memcpy(&currentSpeedValue, attribute.second.data(), sizeof(currentSpeedValue));
            std::cout << "Received Speed Value: " << currentSpeedValue << std::endl;
        }
    }
    valuesUpdated = true;
    cv.notify_all();
}