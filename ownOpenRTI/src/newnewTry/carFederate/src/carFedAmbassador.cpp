#include "../include/carFedAmbassador.h"
#include "../include/carFederate.h"
#include <iostream>
#include <cstring>

carFederateAmbassador::carFederateAmbassador(carFederate& fed) : federate(fed) {}

carFederateAmbassador::~carFederateAmbassador() {
}

void carFederateAmbassador::reflectAttributeValues(
    rti1516e::ObjectInstanceHandle theObject,
    const rti1516e::AttributeHandleValueMap& theAttributes,
    const rti1516e::VariableLengthData& theUserSuppliedTag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType theType,
    rti1516e::LogicalTime const& theTime,
    rti1516e::OrderType receivedOrder,
    rti1516e::MessageRetractionHandle theHandle,
    rti1516e::SupplementalReflectInfo theReflectInfo) {
}

carFederateAmbassador::announceSynchronizationPoint(
    const std::wstring& label, const VariableLengthData& userSuppliedTag) {
    
    std::wcout << L"Received Sync Point Announcement: " << label << std::endl;
    
    if (label == L"SYNC_START") {
        // Notify RTI that this federate has "achieved" the sync point
        rtiAmb->synchronizationPointAchieved(label);
        std::wcout << L"Sync Point Achieved: " << label << std::endl;
    }
}