#include "../include/fedAmbassador.h"
#include "../include/Federate.h"
#include <iostream>
#include <cstring>

FederateAmbassador::FederateAmbassador(Federate& fed) : federate(fed) {}

FederateAmbassador::~FederateAmbassador() {
}

void FederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    const rti1516e::AttributeHandleValueMap& theAttributes,
    const rti1516e::VariableLengthData& theUserSuppliedTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::MessageRetractionHandle theHandle,
    rti1516e::SupplementalReflectInfo theReflectInfo) {

    // Ensure the object handle matches the federate's object instance handle
    if (theObject == federate.vehicleInstanceHandle) {
        // Extract position and speed from the received attribute values
        auto posData = theAttributes.at(federate.positionHandle);
        auto speedData = theAttributes.at(federate.speedHandle);
        
        // Convert the raw byte data into a double
        position = *reinterpret_cast<const double*>(posData.data());
        speed = *reinterpret_cast<const double*>(speedData.data());

        lastReceivedPosition = position;
        lastReceivedSpeed = speed;

        // Print the received values
        std::wcout << "[DE-BUG] Received Position: " << position << ", Speed: " << speed << std::endl;
    }
}

void FederateAmbassador::provideAttributeValueUpdate(
    rti1516e::ObjectInstanceHandle theObject,
    rti1516e::AttributeHandleSet const & theAttributes,
    rti1516e::VariableLengthData const & theTag)
{
    std::wcout << L"[DEBUG] RTI requested attribute value update for: " << theObject << std::endl;
}
