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
#include <numeric>  
#include <sstream> 
#include "../include/shipHelperFunctions.h"

class MyShootShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiambassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";


public: 
    MyShootShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MyShootShipFederateAmbassador();

    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const &theObjectName
    ) override;

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const &theAttributes,
        rti1516e::VariableLengthData const &theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::LogicalTime const & theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReflectInfo theReflectInfo
    ) override;

    void receiveInteraction( //Not used for the moment going to be used for hit event
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo
    ) override;

    void announceSynchronizationPoint(
            std::wstring const& label,
            rti1516e::VariableLengthData const& theUserSuppliedTag
    );

    //Getters and setters for my ship attributehandles
    rti1516e::AttributeHandle getAttributeHandleMyShipPosition() const;
    void setAttributeHandleMyShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMyShipFederateName() const;
    void setAttributeHandleMyShipFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMyShipSpeed() const;
    void setAttributeHandleMyShipSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfRobots() const;
    void setAttributeHandleNumberOfRobots(const rti1516e::AttributeHandle& handle);

    // Getters and setters for enemy ship attributeshandles
    rti1516e::AttributeHandle getAttributeHandleEnemyShipFederateName() const;
    void setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleEnemyShipPosition() const;
    void setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle);

    // ObjectClassHandle set and get
    rti1516e::ObjectClassHandle getMyObjectClassHandle() const;
    void setMyObjectClassHandle(rti1516e::ObjectClassHandle handle);

    //Getters and setters for ship attributes
    std::wstring getMyShipPosition() const;
    void setMyShipPosition(const std::wstring& position);

    std::wstring getMyShipFederateName() const;
    void setMyShipFederateName(const std::wstring& name);

    double getMyShipSpeed() const;
    void setMyShipSpeed(const double& speed);

    int getNumberOfRobots() const;
    void setNumberOfRobots(const int& numRobots);

    std::wstring getEnemyShipFederateName() const;
    void setEnemyShipFederateName(const std::wstring& name);

    //Sync label get
    std::wstring getSyncLabel() const;

    //Enable time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    private:
    //Handles for ship attributes
    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandleMyShipPosition;
    rti1516e::AttributeHandle attributeHandleMyShipFederateName;
    rti1516e::AttributeHandle attributeHandleMyShipSpeed;
    rti1516e::AttributeHandle attributeHandleNumberOfRobots;

    rti1516e::AttributeHandle attributeHandleEnemyShipFederateName;
    rti1516e::AttributeHandle attributeHandleEnemyShipPosition;

    //Ship attributes
    std::wstring myShipPosition = L"";
    std::wstring myShipFederateName = L"";
    double myShipSpeed = 0.0;
    int numberOfRobots = 0;

    std::wstring enemyShipPosition = L"";
    std::wstring enemyShipFederateName = L"";

};