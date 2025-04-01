#ifndef MISSILEFEDERATEAMBASSADOR_H
#define MISSILEFEDERATEAMBASSADOR_H

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

#include "../include/MissileCalculator.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/decodePosition.h"
#include "structMissile.h"


class MissileFederateAmbassador : public rti1516e::NullFederateAmbassador {
private:
    rti1516e::RTIambassador* _rtiAmbassador;

    // Creating missile objects
    std::vector<Missile> missiles;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> missileMap;
    uint64_t missileCounter = 1;
    double wantedHeight = 0.0;

    // Variables used in: announceSynchronizationPoint
    std::wstring syncLabel = L"";
    std::wstring redSyncLabel = L"";
    std::wstring blueSyncLabel = L"";

    // Variables used in: receiveInteraction
    // interactionClassFireMissile
    std::wstring shooterID;
    std::wstring missileTeam;
    std::pair<double, double> missilePosition;
    std::pair<double, double> missileTargetPosition;
    double initialBearing;
    int numberOfMissilesFired;
    double simulationTime = 0.0;    // interactionClassSetupSimulation

    // Variables required from: discoverObjectInstance
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> objectInstanceClassMap;

    // Variables related to time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    rti1516e::ObjectClassHandle objectClassHandleShip;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    rti1516e::AttributeHandle attributeHandleShipTeam;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleFutureShipPosition;    // Might not be nessesary in future.
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipSize;
    rti1516e::AttributeHandle attributeHandleNumberOfMissiles;

    rti1516e::ObjectClassHandle objectClassHandleMissile;
    rti1516e::AttributeHandle attributeHandleMissileID;
    rti1516e::AttributeHandle attributeHandleMissileTeam;
    rti1516e::AttributeHandle attributeHandleMissilePosition;
    rti1516e::AttributeHandle attributeHandleMissileAltitude;
    rti1516e::AttributeHandle attributeHandleMissileSpeed;

    rti1516e::InteractionClassHandle interactionClassSetupSimulation;
    rti1516e::ParameterHandle parameterHandleBlueShips;
    rti1516e::ParameterHandle parameterHandleRedShips;
    rti1516e::ParameterHandle parameterHandleSimulationTime;

    rti1516e::InteractionClassHandle interactionClassFireMissile;
    rti1516e::ParameterHandle parameterHandleShooterID;
    rti1516e::ParameterHandle parameterHandleMissileTeam;
    rti1516e::ParameterHandle parameterHandleMissileStartPosition;
    rti1516e::ParameterHandle parameterHandleMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleMissileSpeed;

    //Parameters and handle for interaction class TargetHit
    rti1516e::InteractionClassHandle interactionClassTargetHit;
    rti1516e::ParameterHandle parameterHandleTargetHitID;
    rti1516e::ParameterHandle parameterHandleTargetHitTeam;
    rti1516e::ParameterHandle parameterHandleTargetHitPosition;
    rti1516e::ParameterHandle parameterHandleTargetHitDestroyed;

public:
    MissileFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MissileFederateAmbassador();

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

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void setWantedHeight(double height);
    double getWantedHeight() const;

    void addNewMissile(rti1516e::ObjectInstanceHandle objectInstanceHandle);
    void removeMissileObject(rti1516e::ObjectInstanceHandle objectInstanceHandle);
    void createNewMissileObject(int numberOfMissiles);

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    // Getters and setters for time management
    std::chrono::time_point<std::chrono::high_resolution_clock> getStartTime() const;
    void setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time);
    bool getIsRegulating() const;
    bool getIsConstrained() const;
    bool getIsAdvancing() const;
    void setIsAdvancing(bool advancing);

    // Getter and setter for missile objects
    std::vector<Missile>& getMissiles();
    void setMissiles(const std::vector<Missile>& missile);

    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t>& getMissileMap();
    void setMissileMap(const std::unordered_map<rti1516e::ObjectInstanceHandle, size_t>& map);

    // Getters and setters for general attributes
    std::wstring getSyncLabel() const;
    std::wstring getRedSyncLabel() const;
    std::wstring getBlueSyncLabel() const;

    // Getter and Setter functins for object class Ship and its attributes
    rti1516e::ObjectClassHandle getObjectClassHandleShip() const;
    void setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleFederateName() const;
    void setAttributeHandleFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipTeam() const;
    void setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipPosition() const;
    void setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleFutureShipPosition() const;
    void setAttributeHandleFutureShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipSpeed() const;
    void setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipSize() const;
    void setAttributeHandleShipSize(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles() const;
    void setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle);

    // Getter and Setter functions for object class Missile and its attributes
    rti1516e::ObjectClassHandle getObjectClassHandleMissile() const;
    void setObjectClassHandleMissile(const rti1516e::ObjectClassHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileID() const;
    void setAttributeHandleMissileID(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileTeam() const;
    void setAttributeHandleMissileTeam(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissilePosition() const;
    void setAttributeHandleMissilePosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileAltitude() const;
    void setAttributeHandleMissileAltitude(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMissileSpeed() const;
    void setAttributeHandleMissileSpeed(const rti1516e::AttributeHandle& handle);  

    // Getter and setter functions for interaction class SetupSimulation
    rti1516e::InteractionClassHandle getInteractionClassSetupSimulation() const;
    void setInteractionClassSetupSimulation(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamTimeScaleFactor() const;
    void setParamTimeScaleFactor(const rti1516e::ParameterHandle& handle);

    // Getter and setter functions for interaction class FireMissile
    rti1516e::InteractionClassHandle getInteractionClassFireMissile() const;
    void setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamShooterID() const;
    void setParamShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileTeam() const;
    void setParamMissileTeam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileStartPosition() const;
    void setParamMissileStartPosition(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileTargetPosition() const;
    void setParamMissileTargetPosition(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamNumberOfMissilesFired() const;
    void setParamNumberOfMissilesFired(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileSpeed() const;
    void setParamMissileSpeed(const rti1516e::ParameterHandle& handle);

    // Getter and setter functions for interaction class MissileFlight
    rti1516e::InteractionClassHandle getInteractionClassMissileFlight() const;
    void setInteractionClassMissileFlight(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightID() const;
    void setParamMissileFlightID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightTeam() const;
    void setParamMissileFlightTeam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightPosition() const;
    void setParamMissileFlightPosition(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightAltitude() const;
    void setParamMissileFlightAltitude(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightSpeed() const;
    void setParamMissileFlightSpeed(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightLockOnTargetID() const;
    void setParamMissileFlightLockOnTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightHitTarget() const;
    void setParamMissileFlightHitTarget(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamMissileFlightDestroyed() const;
    void setParamMissileFlightDestroyed(const rti1516e::ParameterHandle& handle);

    //Get set functions for interaction class TargetHit
    rti1516e::InteractionClassHandle getInteractionClassTargetHit() const;
    void setInteractionClassTargetHit(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamTargetHitID() const;
    void setParamTargetHitID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamTargetHitTeam() const;
    void setParamTargetHitTeam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamTargetHitPosition() const;
    void setParamTargetHitPosition(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamTargetHitDestroyed() const;
    void setParamTargetHitDestroyed(const rti1516e::ParameterHandle& handle);

    // Getters from receiveInteraction variables
    std::wstring getShooterID() const;
    std::wstring getMissileTeam() const;
    std::pair<double, double> getMissilePosition() const;
    void setMissilePosition(const std::pair<double, double>& position);
    std::pair<double, double> getMissileTargetPosition() const;
    int getNumberOfMissilesFired() const;
    double getSimulationTime() const;
    
    // Getters and setters for robot attributes
    double getCurrentDistance() const;
    void setCurrentDistance(const double& distance);

    std::unordered_map<std::wstring, int> TargetMap;
};

#endif