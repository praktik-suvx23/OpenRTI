#ifndef MYSHIPFEDERATEAMBASSADOR_H
#define MYSHIPFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <iostream>

class MyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiAmbassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";

    bool hitStatus = false;
    std::wstring robotID;
    int damageAmount;
public:
    MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MyShipFederateAmbassador();

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag);

    std::wstring getSyncLabel() const;
    std::wstring getFederateName() const;
    void setFederateName(std::wstring name);

    bool getHitStatus() const;
    std::wstring getRobotID() const;
    int getDamageAmount() const;

    // Make Private with get/set methods?
    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::ObjectInstanceHandle objectInstanceHandle;

    rti1516e::AttributeHandle attributeHandleShipTag;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleFutureShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipFederateName;

    rti1516e::InteractionClassHandle hitEventHandle;

    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle damageParam;
};

#endif