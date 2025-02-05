#include "../include/masterFedAmbassador.h"
#include "../include/masterFederate.h"
#include <iostream>
#include <cstring>

masterFedAmbassador::masterFedAmbassador(masterFederate& fed) : federate(fed) {}

masterFedAmbassador::~masterFedAmbassador() {
}

void masterFedAmbassador::reflectAttributeValues(
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

void masterFedAmbassador::synchronizationPointRegistrationSucceeded(const std::wstring& label) {
    std::wcout << L"Sync Point Registered Successfully: " << label << std::endl;
}

void masterFedAmbassador::synchronizationPointRegistrationFailed(const std::wstring& label, rti1516e::SynchronizationPointFailureReason reason) {
    std::wcout << L"Sync Point Registration Failed: " << label << std::endl;
}