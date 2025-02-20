/*
Define the MyFederateAmbassador class inheriting from rti1516e::NullFederateAmbassador.
Declare the constructor and destructor.
Declare methods for handling synchronization points, discovering object instances, and reflecting attribute values.
Declare methods for getting and setting the federate name and sync label.
Declare member variables for object class handles, attribute handles, and robot-specific attributes (e.g., 
currentSpeed, 
currentFuelLevel, 
currentLatitude, 
currentLongitude, 
currentAltitude, 
currentDistance).
*/
#ifndef ROBOTFEDERATEAMBASSADOR_H
#define ROBOTFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <iomanip>
#include <fstream>

#include "../include/Robot.h"
#include "../include/ObjectInstanceHandleHash.h"

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiAmbassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";
public:
    MyFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, const std::wstring &expectedShipName, int instance);
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
        rti1516e::SupplementalReflectInfo theReflectInfo) override;

    std::wstring getSyncLabel() const;
    std::wstring getFederateName() const;
    void setFederateName(std::wstring name);

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

    // HitEvent definitions
    rti1516e::InteractionClassHandle hitEventHandle;
    rti1516e::ParameterHandle robotIDParam;
    rti1516e::ParameterHandle shipIDParam;
    rti1516e::ParameterHandle hitConfirmedParam;

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
    int _instance;
};

#endif