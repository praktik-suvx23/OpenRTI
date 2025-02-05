#include "MyFederateAmbassador.h"
#include <iostream>
#include <cstring>

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
    std::wcout << L"reflectAttributeValues callback invoked" << std::endl;
    for (const auto& attribute : attributeValues) {
        if (attribute.first == attributeHandle) {
            currentAttributeValue = *reinterpret_cast<const uint16_t*>(attribute.second.data());
            std::wcout << L"Received uniqueID Value: " << currentAttributeValue << std::endl;
        }
    }
    valuesUpdated = true;
}


