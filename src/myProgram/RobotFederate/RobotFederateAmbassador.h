#ifndef ROBOTFEDERATEAMBASSADOR_H
#define ROBOTFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>

#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>

#include "../include/Robot.h"
#include "../include/ObjectInstanceHandleHash.h"

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiAmbassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";

    bool assignedTarget = false;
    bool hitStatus = false;
    std::wstring _targetShipID = L"";
    std::wstring shipID = L"";
    int damageAmount;
public:

    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance);
    ~MyFederateAmbassador();

    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag) override;

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const &theObjectName) override;

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const &theAttributes,
        rti1516e::VariableLengthData const &theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::LogicalTime const & theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReflectInfo theReflectInfo) override;

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    std::wstring getSyncLabel() const;
    std::wstring getFederateName() const;
    void setFederateName(std::wstring name);

    bool getHitStatus() const;
    bool getAssignedTarget() const;
    std::wstring getTargetShipID() const;
    std::wstring getShipID() const;
    int getDamageAmount() const;

    // Make Private with get/set methods?
    rti1516e::AttributeHandle attributeHandleFederateName;
    Robot _robot;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _objectInstances;

    // MyShip definitions
    rti1516e::ObjectClassHandle shipClassHandle;
    rti1516e::AttributeHandle attributeHandleShipTag;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleFutureShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    rti1516e::AttributeHandle attributeHandleShipSize;
    rti1516e::AttributeHandle attributeHandleNumberOfRobots;
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    std::wstring _expectedPublisherName;
    std::wstring _expectedShipName;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    double simulationTime = 0.0;

    // HitEvent definitions
    rti1516e::InteractionClassHandle hitEventHandle;
    // Parameters
    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle damageParam;

    // Ex
    std::wstring RobotPosition;
    std::wstring shipPosition;

    bool firstPosition = true;
    bool heightAchieved = false;
    double currentDistance;

    double shipSize = 0.0;
    int numberOfRobots = 0;

    double currentSpeed = 0.0;
    double currentFuelLevel = 100.0;
    double currentLatitude = 0.0;
    double currentLongitude = 0.0;
    std::wstring currentPosition = std::to_wstring(currentLatitude) + L"," + std::to_wstring(currentLongitude);
    double currentAltitude = 0.0;

    std::wstring expectedFuturePosition;
    std::wstring expectedShipPosition;
    int instance;

    bool isRegulating = false;  
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
};

#endif