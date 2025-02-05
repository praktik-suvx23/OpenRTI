#ifndef MYFEDERATEAMBASSADOR_H
#define MYFEDERATEAMBASSADOR_H

#include <RTI/NullFederateAmbassador.h>
#include <RTI/RTI1516.h>
#include <iostream>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    MyFederateAmbassador() : valuesUpdated(false) {}

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle objectInstanceHandle,
        const rti1516e::AttributeHandleValueMap& attributeValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& time,
        rti1516e::OrderType receivedOrder,
        rti1516e::MessageRetractionHandle retractionHandle,
        rti1516e::SupplementalReflectInfo reflectInfo) override;

    rti1516e::AttributeHandle attributeHandle;
    uint16_t currentAttributeValue;
    bool valuesUpdated;
};

#endif // MYFEDERATEAMBASSADOR_H