#ifndef MISSILEMANAGERAMBASSADOR_H
#define MISSILEMANAGERAMBASSADOR_H

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

#include "../include/Robot.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "include/ActiveMissile.h"

class MissileManagerAmbassador : public rti1516e::NullFederateAmbassador {
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

    /* I beleive only MissileID, Pos/Alti and MissileTeam are the only attributes required.
    Because the other team *shouldn't know what ship the missile is targeting.
    Because with missile position the defending ship can calculate speed.
    FuelLevel is most likely unnecessary.   */
    rti1516e::ObjectClassHandle missileClassHandle;             // Object class handle for missile
    rti1516e::AttributeHandle attributeHandleMissileID;         // Attribute handle for missile ID
    rti1516e::AttributeHandle attributeHandleSpeed;             // Attribute handle for missile speed
    rti1516e::AttributeHandle attributeHandleFuelLevel;         // Attribute handle for missile fuel level.
    rti1516e::AttributeHandle attributeHandlePosition;          // Attribute handle for missile position
    rti1516e::AttributeHandle attributeAltitude;                // Attribute handle for missile altitude
    rti1516e::AttributeHandle attributeHandleTarget;            // Attribute handle for missile target
    rti1516e::AttributeHandle attributeHandleMissileTeam;       // Attribute handle for missile team

    rti1516e::RTIambassador* _rtiAmbassador;        //To get access to the RTIambassador
    std::wstring federateName = L"";                //Name of the current federate
    std::wstring syncLabel = L"";                   //Synchronization point label
    

    // Variables used in receiveInteraction
    bool flagActiveMissile = false;
    std::wstring shooterID, targetID, missileType;
    double missileSpeed = 0.0, maxDistance = 0.0, lockOnDistance = 0.0, fireTime = 0.0;
    int missileCount = 0;
    std::pair<double, double> shooterPosition, targetPosition;  // X, Y position of the shooting and target ship

    std::wstring _expectedShipName;                 // ?
    std::wstring TargetFederate;                    // ?

    std::wstring RobotPosition;                     // ?
    std::wstring shipPosition;                      // ?
    std::wstring expectedFuturePosition;            // ?
    std::wstring expectedShipPosition;              // ?
    
    double shipSize = 0.0;                          // ?
    int numberOfRobots = 0;                         // ?
    double currentDistance;                         // ?

    std::wstring currentPosition;                   // ?
    double currentAltitude = 0.0;                   // ?
    double currentSpeed = 0.0;                      // ?
    double currentFuelLevel = 100.0;                // ?

    // Might be replaced by fireMissileHandle shortly.
    rti1516e::InteractionClassHandle fireRobotHandle;   // Interaction class handle for fire robot
    rti1516e::ParameterHandle fireParamHandle;          // Parameter handle for fire
    rti1516e::ParameterHandle targetParamHandle;        // Parameter handle for target
    rti1516e::ParameterHandle startPosRobot;            // Parameter handle for start position of robot
public:

    MissileManagerAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MissileManagerAmbassador();

    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag) override;

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReceiveInfo receiveInfo
    ) override;

    // Getters and setters for general attributes
    std::wstring getSyncLabel() const;
    std::wstring getFederateName() const;
    void setFederateName(std::wstring name);

    // Getters and setters for _expectedShipName
    std::wstring getExpectedShipName() const;
    void setExpectedShipName(const std::wstring& name);

    // Getters and setters for TargetFederate
    std::wstring getTargetFederate() const;
    void setTargetFederate(const std::wstring& federate);

    // Getters and setters for RobotPosition
    std::wstring getRobotPosition() const;
    void setRobotPosition(const std::wstring& position);

    // Getters and setters for expectedFuturePosition
    std::wstring getExpectedFuturePosition() const;
    void setExpectedFuturePosition(const std::wstring& position);

    // Getters and setters for activeMissiles
    std::vector<ClassActiveMissile> getActiveMissiles() const;
    void setActiveMissiles(const std::vector<ClassActiveMissile>& missiles);

    //Get and set for fire interaction
    rti1516e::InteractionClassHandle getFireRobotHandle() const;
    void setFireRobotHandle(const rti1516e::InteractionClassHandle& handle);
    rti1516e::ParameterHandle getFireRobotHandleParam() const;
    void setFireRobotHandleParam(const rti1516e::ParameterHandle& handle);
    rti1516e::ParameterHandle getTargetParam() const;
    void setTargetParam(const rti1516e::ParameterHandle& handle);
    rti1516e::ParameterHandle getStartPosRobot() const;
    void setStartPosRobot(const rti1516e::ParameterHandle& handle);

    // Getters and setters for robot attributes
    double getCurrentAltitude() const;
    void setCurrentAltitude(double altitude);

    double getCurrentSpeed() const;
    void setCurrentSpeed(const double& speed);

    double getCurrentFuelLevel() const;
    void setCurrentFuelLevel(const double& fuelLevel);

    std::wstring getCurrentPosition() const;
    void setCurrentPosition(const std::wstring& position);

    double getCurrentDistance() const;
    void setCurrentDistance(const double& distance);

    std::wstring getShipPosition() const;
    void setShipPosition(const std::wstring& position);

    std::wstring getExpectedShipPosition() const;
    void setExpectedShipPosition(const std::wstring& position);

    double getShipSize() const;
    void setShipSize(const double& size);

    int getNumberOfRobots() const;
    void setNumberOfRobots(const int& robots);

    std::wstring getCurrentPosition() const;
    void setCurrentPosition(const std::wstring& position);

    double getCurrentDistance() const;
    void setCurrentDistance(const double& distance);

    std::wstring getShipPosition() const;
    void setShipPosition(const std::wstring& position);

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime; // Not currently in use
    int instance;   // Not currently in use

    bool isRegulating = false;  
    bool isConstrained = false;
    bool isAdvancing = false;

    // Active missiles in the simulation. Create get/set methods for this IF we're going to use this.
    std::vector<ClassActiveMissile> activeMissiles; 

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    // Getters and setters for interaction class and parameter handles
    rti1516e::InteractionClassHandle getFireMissileHandle() const;
    void setFireMissileHandle(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getShooterIDParamHandle() const;
    void setShooterIDParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getTargetIDParamHandle() const;
    void setTargetIDParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getShooterPositionParamHandle() const;
    void setShooterPositionParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getTargetPositionParamHandle() const;
    void setTargetPositionParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getMissileCountParamHandle() const;
    void setMissileCountParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getMissileTypeParamHandle() const;
    void setMissileTypeParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getMaxDistanceParamHandle() const;
    void setMaxDistanceParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getMissileSpeedParamHandle() const;
    void setMissileSpeedParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getLockOnDistanceParamHandle() const;
    void setLockOnDistanceParamHandle(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getFireTimeParamHandle() const;
    void setFireTimeParamHandle(const rti1516e::ParameterHandle& handle);

    // Getters and setters for object class and attribute handles
    rti1516e::ObjectClassHandle getShipClassHandle() const;
    void setShipClassHandle(const rti1516e::ObjectClassHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipID() const;
    void setAttributeHandleShipID(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipTeam() const;
    void setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipPosition() const;
    void setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipSpeed() const;
    void setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipSize() const;
    void setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipAngle() const;
    void setAttributeHandleShipAngle(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles() const;
    void setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle);

    rti1516e::ObjectClassHandle getMissileClassHandle() const;
    void setMissileClassHandle(const rti1516e::ObjectClassHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileID() const;
    void setAttributeHandleMissileID(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleSpeed() const;
    void setAttributeHandleSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleFuelLevel() const;
    void setAttributeHandleFuelLevel(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandlePosition() const;
    void setAttributeHandlePosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeAltitude() const;
    void setAttributeAltitude(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleTarget() const;
    void setAttributeHandleTarget(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileTeam() const;
    void setAttributeHandleMissileTeam(const rti1516e::AttributeHandle& handle);
};

#endif