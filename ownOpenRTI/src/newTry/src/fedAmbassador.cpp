#include "../include/fedAmbassador.h"
#include <iostream>

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

void FederateAmbassador::reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                                rti1516e::AttributeHandleValueMap const &theAttributes,
                                                rti1516e::VariableLengthData const &theUserSuppliedTag,
                                                rti1516e::OrderType sentOrder,
                                                rti1516e::TransportationType theType,
                                                rti1516e::LogicalTime const &theTime,
                                                rti1516e::OrderType receivedOrder,
                                                rti1516e::MessageRetractionHandle theHandle,
                                                rti1516e::SupplementalReflectInfo theReflectInfo) {
    std::cout << "Attribute values reflected." << std::endl;
}

void FederateAmbassador::setVehiclePosition(double position) {
    _vehiclePosition = position;
}

double FederateAmbassador::getVehiclePosition() const {
    return _vehiclePosition;
}

void FederateAmbassador::setVehicleSpeed(double speed) {
    _vehicleSpeed = speed;
}

double FederateAmbassador::getVehicleSpeed() const {
    return _vehicleSpeed;
}