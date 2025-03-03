#ifndef MYSHIPFEDERATEAMBASSADOR_H
#define MYSHIPFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>

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

    // Getter and Setter functions for attribute handles
    rti1516e::AttributeHandle getAttributeHandleShipTag() const;
    void setAttributeHandleShipTag(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleShipPosition() const;
    void setAttributeHandleShipPosition(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleFutureShipPosition() const;
    void setAttributeHandleFutureShipPosition(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleShipSpeed() const;
    void setAttributeHandleShipSpeed(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleShipFederateName() const;
    void setAttributeHandleShipFederateName(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleShipSize() const;
    void setAttributeHandleShipSize(rti1516e::AttributeHandle handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfRobots() const;
    void setAttributeHandleNumberOfRobots(rti1516e::AttributeHandle handle);

    // getter and setter for ObjectClassHandle
    rti1516e::ObjectClassHandle getMyObjectClassHandle() const;
    void setMyObjectClassHandle(rti1516e::ObjectClassHandle handle);

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag
    );

    // Setters and getters for ship attributes
    std::wstring getSyncLabel() const;

    void setFederateName(std::wstring name);
    std::wstring getFederateName() const;

    void setshipNumber(std::wstring name);
    std::wstring getshipNumber() const;

    void setshipheight(double height);
    double getshipheight() const;

    void setshipwidth(double width);
    double getshipwidth() const ;

    void setshiplength(double length);
    double getshiplength() const;

    void setNumberOfRobots(int numRobots);
    int getNumberOfRobots() const;

    double getShipSize();

    // Getters for hit event
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
    rti1516e::AttributeHandle attributeHandleShipSize;
    rti1516e::AttributeHandle attributeHandleNumberOfRobots;

    rti1516e::InteractionClassHandle hitEventHandle;

    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle damageParam;

    std::wstring shipNumber;
    double shipheight;
    double shipwidth;
    double shiplength;
    double ShipSize;
    int numberOfRobots;

    bool isRegulating = false;  
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

};

#endif