#ifndef FEDERATEAMBASSADOR_H
#define FEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>

class FederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    FederateAmbassador();
    ~FederateAmbassador();

    void receiveInteraction(rti1516e::InteractionClassHandle theInteraction,
                            rti1516e::ParameterHandleValueMap const &theParameters,
                            rti1516e::VariableLengthData const &theUserSuppliedTag,
                            rti1516e::OrderType sentOrder,
                            rti1516e::TransportationType theType,
                            rti1516e::LogicalTime const &theTime,
                            rti1516e::OrderType receivedOrder,
                            rti1516e::MessageRetractionHandle theHandle,
                            rti1516e::SupplementalReceiveInfo theReceiveInfo) override;

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                rti1516e::AttributeHandleValueMap const &theAttributes,
                                rti1516e::VariableLengthData const &theUserSuppliedTag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType theType,
                                rti1516e::LogicalTime const &theTime,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle theHandle,
                                rti1516e::SupplementalReflectInfo theReflectInfo) override;

    void setVehiclePosition(double position);
    double getVehiclePosition() const;

    void setVehicleSpeed(double speed);
    double getVehicleSpeed() const;

private:
    int _callbackState;
    double _vehiclePosition;
    double _vehicleSpeed;
};

#endif