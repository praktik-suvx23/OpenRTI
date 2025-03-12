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

#include "../include/ObjectInstanceHandleHash.h"
#include "../include/Robot.h"

class MyShootShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiambassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";
    Robot myShip;
    
    rti1516e::InteractionClassHandle fireMissileHandle;     // Interaction class handle for fire missile
    rti1516e::ParameterHandle shooterIDParamHandle;         // ID of the shooting ship
    rti1516e::ParameterHandle targetIDParamHandle;          // ID of the target ship
    rti1516e::ParameterHandle shooterPositionParamHandle;   // X, Y position of the shooting ship
    rti1516e::ParameterHandle targetPositionParamHandle;    // X, Y position of the target ship
    rti1516e::ParameterHandle missileCountParamHandle;      // Number of missiles to fire
    rti1516e::ParameterHandle missileTypeParamHandle;       // TODO: Implement missile type
    rti1516e::ParameterHandle maxDistanceParamHandle;       // TODO: Implement max travel distance
    rti1516e::ParameterHandle missileSpeedParamHandle;      // TODO: Improve missile speed
    rti1516e::ParameterHandle lockOnDistanceParamHandle;    // TODO: Improve lock function
    rti1516e::ParameterHandle fireTimeParamHandle;          // Might not be nessesary

    rti1516e::ObjectClassHandle shipClassHandle;                // Object class handle for ship
    rti1516e::AttributeHandle attributeHandleShipID;            // Attribute handle for ship ID / Name
    rti1516e::AttributeHandle attributeHandleShipTeam;          // Attribute handle for ship team
    rti1516e::AttributeHandle attributeHandleShipPosition;      // Attribute handle for ship position
    rti1516e::AttributeHandle attributeHandleShipSpeed;         // Attribute handle for ship speed
    rti1516e::AttributeHandle attributeHandleShipSize;          // Attribute handle for ship size
    rti1516e::AttributeHandle attributeHandleShipAngle;         // Attribute handle for ship angle. Not currently in use. Just an idea
    rti1516e::AttributeHandle attributeHandleNumberOfMissiles;  // Attribute handle for number of missiles
public: 
    MyShootShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador, int instance);
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

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint(
            std::wstring const& label,
            rti1516e::VariableLengthData const& theUserSuppliedTag
    );
    // Getter Methods for interaction class and parameters
    rti1516e::InteractionClassHandle getFireMissileHandle() const;
    rti1516e::ParameterHandle getShooterIDParamHandle() const;
    rti1516e::ParameterHandle getTargetIDParamHandle() const;
    rti1516e::ParameterHandle getShooterPositionParamHandle() const;
    rti1516e::ParameterHandle getTargetPositionParamHandle() const;
    rti1516e::ParameterHandle getMissileCountParamHandle() const;
    rti1516e::ParameterHandle getMissileTypeParamHandle() const;
    rti1516e::ParameterHandle getMaxDistanceParamHandle() const;
    rti1516e::ParameterHandle getMissileSpeedParamHandle() const;
    rti1516e::ParameterHandle getLockOnDistanceParamHandle() const;
    rti1516e::ParameterHandle getFireTimeParamHandle() const;
    // Setter Methods for interaction class and parameters
    void setFireMissileHandle(rti1516e::InteractionClassHandle handle);
    void setShooterIDParamHandle(rti1516e::ParameterHandle handle);
    void setTargetIDParamHandle(rti1516e::ParameterHandle handle);
    void setShooterPositionParamHandle(rti1516e::ParameterHandle handle);
    void setTargetPositionParamHandle(rti1516e::ParameterHandle handle);
    void setMissileCountParamHandle(rti1516e::ParameterHandle handle);
    void setMissileTypeParamHandle(rti1516e::ParameterHandle handle);
    void setMaxDistanceParamHandle(rti1516e::ParameterHandle handle);
    void setMissileSpeedParamHandle(rti1516e::ParameterHandle handle);
    void setLockOnDistanceParamHandle(rti1516e::ParameterHandle handle);
    void setFireTimeParamHandle(rti1516e::ParameterHandle handle);

    // Getter Metods for object class and attributes
    rti1516e::ObjectClassHandle getObjectClassHandleShip() const;
    rti1516e::AttributeHandle getAttributeHandleShipID() const;
    rti1516e::AttributeHandle getAttributeHandleShipTeam() const;
    rti1516e::AttributeHandle getAttributeHandleShipPosition() const;
    rti1516e::AttributeHandle getAttributeHandleShipSpeed() const;
    rti1516e::AttributeHandle getAttributeHandleShipSize() const;
    rti1516e::AttributeHandle getAttributeHandleShipAngle() const;
    rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles() const;
    // Setter Methods for object class and attributes
    void setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle);
    void setAttributeHandleShipID(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle);
    void setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle);

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

    //Get and set for fire interaction
    rti1516e::InteractionClassHandle getFireRobotHandle() const;
    void setFireRobotHandle(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getFireRobotHandleParam() const;
    void setFireRobotHandleParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getTargetParam() const;
    void setTargetParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getStartPosRobot() const;
    void setStartPosRobot(const rti1516e::ParameterHandle& handle);

    //Getters and setters for ship attributes
    std::wstring getMyShipPosition() const;
    void setMyShipPosition(const std::wstring& position);

    std::wstring getMyShipFederateName() const;
    void setMyShipFederateName(const std::wstring& name);

    double getMyShipSpeed() const;
    void setMyShipSpeed(const double& speed);

    std::wstring getEnemyShipFederateName() const;
    void setEnemyShipFederateName(const std::wstring& name);

    std::wstring getEnemyShipPosition() const;
    void setEnemyShipPosition(const std::wstring& position);

    double getDistanceBetweenShips() const;
    void setDistanceBetweenShips(const double& distance);

    double getBearing() const;
    void setBearing(const double& bearing);

    bool getIsFiring() const;
    void setIsFiring(const bool& firing);
    
    //Json values get/set
    std::wstring getshipNumber() const;
    void setshipNumber(const std::wstring& name);

    double getshipheight() const;
    void setshipheight(const double& height);

    double getshipwidth() const;
    void setshipwidth(const double& width);

    double getshiplength() const;
    void setshiplength(const double& length);

    double getShipSize();

    int getNumberOfRobots() const;
    void setNumberOfRobots(const int& numRobots);

    //Sync label get
    std::wstring getSyncLabel() const;

    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    int instance = 0;
    //Enable time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    rti1516e::ObjectInstanceHandle objectInstanceHandle;

    private:

    //Json values
    std::wstring shipNumber;
    double shipheight;
    double shipwidth;
    double shiplength;
    double ShipSize;
    int numberOfRobots;

    //Standard values
    bool isFiring = false;
    double distanceBetweenShips = 0.0;
    double bearing = 0.0;
    std::wstring _expectedShipName;

    //Interaction send params and handle
    rti1516e::InteractionClassHandle fireRobotHandle;
    rti1516e::ParameterHandle fireParamHandle;
    rti1516e::ParameterHandle TargetParam;
    rti1516e::ParameterHandle startPosRobot;

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

    std::wstring enemyShipPosition = L"";
    std::wstring enemyShipFederateName = L"";

    bool shipAlive = true;
    bool targetAlive = true;
};