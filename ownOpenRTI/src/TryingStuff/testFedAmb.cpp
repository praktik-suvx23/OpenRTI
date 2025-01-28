#include <iostream>
#include <RTI/time/HLAfloat64Interval.h>
#include "testFedAmb.h"

testFedAmb::testFedAmb() 
    : federateTime(0.0), 
      federateLookahead(1.0), 
      isRegulating(false), 
      isConstrained(false), 
      isAdvancing(false), 
      isAnnounced(false), 
      isReadyToRun(false) {
    // Constructor implementation
}

testFedAmb::~testFedAmb(){
    
}

// 1. Instance methods
double testFedAmb::convertTime(const rti1516e::LogicalTime& theTime){
    const rti1516e::HLAfloat64Time& floatTime = dynamic_cast<const rti1516e::HLAfloat64Time&>(theTime);
    return floatTime.getTime();
}

// 2. Synchrnization point methods
void testFedAmb::synchronizationPointRegistrationSucceeded(const std::wstring& label)
        throw (rti1516e::FederateInternalError){
    std::wcout << L"Successfully registered sync point: " << label << std::endl;
}

void testFedAmb::synchronizationPointRegistrationFailed(const std::wstring& label)
        throw (rti1516e::FederateInternalError){
    std::wcout << L"Failed to register sync point: " << label << std::endl;
}

void testFedAmb::announceSynchronizationPoint(const std::wstring& label, const rti1516e::VariableLengthData& tag)
        throw (rti1516e::FederateInternalError){
    std::wcout << L"Received sync point announcement: " << label << std::endl;
    if(label.compare(READY_TO_RUN) == 0){
        this->isAnnounced = true;
    }
}

void testFedAmb::federationSynchronized(const std::wstring& label, const rti1516e::FederateHandleSet& failedToSyncSet)
        throw (rti1516e::FederateInternalError){
    std::wcout << L"Federation Synchronized: " << label << std::endl;
    if(label.compare(READY_TO_RUN) == 0){
        this->isReadyToRun = true;
    }
}

// 3. Time management methods
void testFedAmb::timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime)
        throw (rti1516e::InvalidFederationTime, rti1516e::EnableTimeRegulationWasNotPending, rti1516e::FederateInternalError){
    this->federateTime = convertTime(theFederateTime);
    this->isRegulating = true;
}

void testFedAmb::timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime)
        throw (rti1516e::InvalidFederationTime, rti1516e::EnableTimeConstrainedWasNotPending, rti1516e::FederateInternalError){
    this->federateTime = convertTime(theFederateTime);
    this->isConstrained = true;
}

void testFedAmb::timeAdvanceGrant(const rti1516e::LogicalTime& theTime)
        throw (rti1516e::InvalidFederationTime, rti1516e::TimeAdvanceWasNotInProgress, rti1516e::FederateInternalError){
    this->federateTime = convertTime(theTime);
    this->isAdvancing = false;
}

// 4. Object Management Callbacks
// 4.1 Discover object methods
void testFedAmb::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,const std::wstring& theObjectName)
        throw (rti1516e::CouldNotDiscover, rti1516e::ObjectClassNotKnown, rti1516e::FederateInternalError){
    std::wcout << L"Discoverd object: handle=" << theObject
    << L", classHandle=" << theObjectClass
    << L", name=" << theObjectName << std::endl;
}

void testFedAmb::discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,const std::wstring& theObjectName, rti1516e::FederateHandle producingFederate)
        throw (rti1516e::CouldNotDiscover, rti1516e::ObjectClassNotKnown, rti1516e::FederateInternalError){
    std::wcout << L"Discoverd object: handle=" << theObject
    << L", classHandle=" << theObjectClass
    << L", name=" << theObjectName
    << L", producing federate=" << producingFederate << std::endl;
}

// 4.2 Reflect attribute value methods
void testFedAmb::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::AttributeHandleValueMap& theAttributeValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        SupplementalReflectInfo theReflectInfo)
        throw ( FederateInternalError ){
    // Print handle and attribute information
    std::wcout << L"Reflecting attribute values for object: handle=" << theObject 
    << L", attributeCount=" << theAttributeValues.size() std::endl;
    for(rti1516e::AttributeHandleValueMap::const_iterator i = theAttributeValues.begin(); i != theAttributeValues.end(); ++i){
        // Print the attribute handle
        std::wcout << L"Attribute handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

void testFedAmb::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::AttributeHandleValueMap& theAttributeValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        rti1516e::LogicalTime theTime, rti1516e::OrderType receivedOrder,
        SupplementalReflectInfo theReflectInfo)
        throw ( FederateInternalError ){
    // Print handle and attribute information
    std::wcout << L"Reflecting attribute values for object: handle=" << theObject 
    << L", attributeCount=" << theAttributeValues.size() std::endl;
    for(rti1516e::AttributeHandleValueMap::const_iterator i = theAttributeValues.begin(); i != theAttributeValues.end(); ++i){
        // Print the attribute handle
        std::wcout << L"Attribute handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

void testFedAmb::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::AttributeHandleValueMap& theAttributeValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        rti1516e::LogicalTime theTime, rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle retractionHandle,
        SupplementalReflectInfo theReflectInfo)
        throw ( FederateInternalError ){
    // Print handle and attribute information
    std::wcout << L"Reflecting attribute values for object: handle=" << theObject 
    << L", attributeCount=" << theAttributeValues.size() std::endl;
    for(rti1516e::AttributeHandleValueMap::const_iterator i = theAttributeValues.begin(); i != theAttributeValues.end(); ++i){
        // Print the attribute handle
        std::wcout << L"Attribute handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

// 4.3 Receive interaction methods
void testFedAmb::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
        const rti1516e::ParameterHandleValueMap& theParameterValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        SupplementalReceiveInfo theReceiveInfo)
        throw ( FederateInternalError ){
    // Print handle and parameter information
    std::wcout << L"Received interaction: handle=" << theInteraction 
    << L", parameterCount=" << theParameterValues.size() std::endl;
    for(rti1516e::ParameterHandleValueMap::const_iterator i = theParameterValues.begin(); i != theParameterValues.end(); ++i){
        // Print the parameter handle
        std::wcout << L"Parameter handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

void testFedAmb::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
        const rti1516e::ParameterHandleValueMap& theParameterValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        rti1516e::LogicalTime theTime, rti1516e::OrderType receivedOrder,
        SupplementalReceiveInfo theReceiveInfo)
        throw ( FederateInternalError ){
    // Print handle and parameter information
    std::wcout << L"Received interaction: handle=" << theInteraction 
    << L", parameterCount=" << theParameterValues.size() std::endl;
    for(rti1516e::ParameterHandleValueMap::const_iterator i = theParameterValues.begin(); i != theParameterValues.end(); ++i){
        // Print the parameter handle
        std::wcout << L"Parameter handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

void testFedAmb::receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
        const rti1516e::ParameterHandleValueMap& theParameterValues,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::TransportationType theType,
        rti1516e::LogicalTime theTime, rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle retractionHandle,
        SupplementalReceiveInfo theReceiveInfo)
        throw ( FederateInternalError ){
    // Print handle and parameter information
    std::wcout << L"Received interaction: handle=" << theInteraction 
    << L", parameterCount=" << theParameterValues.size() std::endl;
    for(rti1516e::ParameterHandleValueMap::const_iterator i = theParameterValues.begin(); i != theParameterValues.end(); ++i){
        // Print the parameter handle
        std::wcout << L"Parameter handle=" << i->first << std::endl;
        for(rti1516e::VariableLengthData::const_iterator j = i->second.begin(); j != i->second.end(); ++j){
            std::wcout << L"Value=" << *j << std::endl;
        }
    }
}

// 5. Remove object model
void testFedAmb::removeObjectInstance(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, SupplementalRemoveInfo theRemoveInfo)
        throw ( FederateInternalError ){
    std::wcout << L"Object removed: handle=" << theObject << std::endl;
}

void testFedAmb::removeObjectInstance(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::LogicalTime theTime,
        rti1516e::OrderType receivedOrder, SupplementalRemoveInfo theRemoveInfo)
        throw ( FederateInternalError ){
    std::wcout << L"Object removed: handle=" << theObject << std::endl;
}

void testFedAmb::removeObjectInstance(rti1516e::ObjectInstanceHandle theObject,
        const rti1516e::VariableLengthData& theUserSuppliedTag,
        rti1516e::OrderType sentOrder, rti1516e::LogicalTime theTime,
        rti1516e::OrderType receivedOrder, rti1516e::MessageRetractionHandle retractionHandle,
        SupplementalRemoveInfo theRemoveInfo)
        throw ( FederateInternalError ){
    std::wcout << L"Object removed: handle=" << theObject << std::endl;
}