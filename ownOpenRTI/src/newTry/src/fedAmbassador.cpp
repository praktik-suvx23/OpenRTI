#include "../include/fedAmbassador.h"
#include <iostream>
#include <cstring>

FederateAmbassador::FederateAmbassador(Federate& fed) : federate(fed) {}

FederateAmbassador::~FederateAmbassador() {
}

void FederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle objectHandle,
    const rti1516e::AttributeHandleValueMap& attributes,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType type,
    const rti1516e::LogicalTime& time,
    const rti1516e::OrderType receivedOrder,
    rti1516e::MessageRetractionHandle theHandle,
    rti1516e::SupplementalReflectInfo theReflectInfo) 
{
    for (const auto& attr : attributes) {
        rti1516e::AttributeHandle attrHandle = attr.first;
        const rti1516e::VariableLengthData& attrValue = attr.second;

        if (attrValue.size() == sizeof(double)) {  // Ensure correct data size
            if (attrHandle == federate.positionHandle) {
                federate.position = *reinterpret_cast<const double*>(attrValue.data());
                std::cout << "Subscribed Position: " << federate.position << std::endl;
            } 
            else if (attrHandle == federate.speedHandle) {
                federate.speed = *reinterpret_cast<const double*>(attrValue.data());
                std::cout << "Subscribed Speed: " << federate.speed << std::endl;
            }
        } else {
            std::cerr << "Error: Received attribute with incorrect size!" << std::endl;
        }
    }
}