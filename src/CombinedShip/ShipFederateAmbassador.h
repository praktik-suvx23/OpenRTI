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
#include <unistd.h>

#include "../include/decodePosition.h"
#include "../include/jsonParse.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/shipHelperFunctions.h"
#include "../include/MissileCalculator.h"
#include "../include/loggingFunctions.h"
#include "Ship.h"

class MyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiambassador;
    std::wstring federateName = L"";
    ShipTeam teamStatus = ShipTeam::UNSIGNED;
    std::wstring syncLabel = L"";
    std::wstring redSyncLabel = L"";
    std::wstring blueSyncLabel = L"";

    int numberOfDiscoveredObjects = 1;

    // Ship objects
    std::unordered_map<rti1516e::ObjectInstanceHandle, Ship> shipMap;
    std::vector<Ship*> blueShipsVector;
    std::vector<Ship*> redShipsVector;
    std::vector<Ship*> ownShipsVector;
    std::vector<FireOrder> fireOrders;
    // Map: shipID, targetShipID
    std::map<Ship*, Ship*> closestEnemyship;

    //Datavalues for setup
    int shipCounter = 0;
    int amountOfMissiles = 0;
    double timeScale = 0;

    //Standard values
    bool isFiring = false;
    double distanceBetweenShips = 0.0;
    double bearing = 0.0;
    std::wstring enemyShipFederateName = L"";
    std::pair<double, double> enemyShipPosition = std::make_pair(0.0, 0.0);

    // createShips bool
    bool createShips = false;

    // Time related bools
    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    //Handles for setup simulation interaction
    rti1516e::InteractionClassHandle setupSimulationHandle;
    rti1516e::ParameterHandle blueShips;
    rti1516e::ParameterHandle redShips;
    rti1516e::ParameterHandle timeScaleFactor;

    //Handles for ship attributes
    rti1516e::ObjectClassHandle objectClassHandleShip;
    rti1516e::AttributeHandle attributeHandleShipFederateName;
    rti1516e::AttributeHandle attributeHandleShipTeam;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleNumberOfMissiles;
    rti1516e::AttributeHandle attributeHandleMaxMissilesLockingThisShip;
    rti1516e::AttributeHandle attributeHandleCurrentMissilesLockingThisShip;

    //Handles for interaction class FireMissile
    rti1516e::InteractionClassHandle interactionClassFireMissile;
    rti1516e::ParameterHandle parameterHandleShooterID;
    rti1516e::ParameterHandle parameterHandleTargetID;
    rti1516e::ParameterHandle parameterHandleMissileTeam;
    rti1516e::ParameterHandle parameterHandleMissileStartPosition;
    rti1516e::ParameterHandle parameterHandleMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleMissileSpeed;  // not in use

    //Handles for interaction class initiate handshake
    rti1516e::InteractionClassHandle interactionClassInitiateRedHandshake;
    rti1516e::InteractionClassHandle interactionClassInitiateBlueHandshake;
    rti1516e::ParameterHandle parameterHandleInitiateShooterID;
    rti1516e::ParameterHandle parameterHandleInitiateMissileAmountFired;
    rti1516e::ParameterHandle parameterHandleInitiateTargetID;
    rti1516e::ParameterHandle parameterHandleMaxMissilesRequired;
    rti1516e::ParameterHandle parameterHandleCurrentlyTargeting;
    rti1516e::ParameterHandle parameterHandleDistanceToTarget;

    //Handles for interaction class confirm handshake
    rti1516e::InteractionClassHandle interactionClassConfirmRedHandshake;
    rti1516e::InteractionClassHandle interactionClassConfirmBlueHandshake;
    rti1516e::ParameterHandle parameterHandleConfirmShooterID;
    rti1516e::ParameterHandle parameterHandleConfirmMissileAmountFired;
    rti1516e::ParameterHandle parameterHandleConfirmTargetID;
    rti1516e::ParameterHandle parameterHandleConfirmAllowFire;

    //Parameters and handle for interaction class TargetHit
    rti1516e::InteractionClassHandle interactionClassTargetHit;
    rti1516e::ParameterHandle parameterHandleTargetHitID;
    rti1516e::ParameterHandle parameterHandleTargetHitTeam;
    rti1516e::ParameterHandle parameterHandleTargetHitPosition;
    rti1516e::ParameterHandle parameterHandleTargetHitDestroyed;

    void readJsonFile(Ship* ship);
public: 
    MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MyShipFederateAmbassador();

    void setFederateName(const std::wstring& name);
    std::wstring getFederateName() const;

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
    void createNewShips(int amountOfShips);
    void addShip(rti1516e::ObjectInstanceHandle objectHandle);

    //Getters and setters for my ship attributehandles
    rti1516e::ObjectClassHandle getObjectClassHandleShip() const;
    void setObjectClassHandleShip(rti1516e::ObjectClassHandle handle);

    rti1516e::AttributeHandle getAttributeHandleShipFederateName() const;
    void setAttributeHandleShipFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipTeam() const;
    void setAttributeHandleShipTeam(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipPosition() const;
    void setAttributeHandleShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleShipSpeed() const;
    void setAttributeHandleShipSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles() const;
    void setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMaxMissilesLockingThisShip() const;
    void setAttributeHandleMaxMissilesLockingThisShip(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleCurrentMissilesLockingThisShip() const;
    void setAttributeHandleCurrentMissilesLockingThisShip(const rti1516e::AttributeHandle& handle);

    // Getters and setters for enemy ship attributeshandles
    rti1516e::AttributeHandle getAttributeHandleEnemyShipFederateName() const;
    void setAttributeHandleEnemyShipFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleEnemyShipPosition() const;
    void setAttributeHandleEnemyShipPosition(const rti1516e::AttributeHandle& handle);

    //Get and set for setup simulation interaction
    rti1516e::InteractionClassHandle getSetupSimulationHandle() const;
    void setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getBlueShipsParam() const;
    void setBlueShipsParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getRedShipsParam() const;
    void setRedShipsParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getTimeScaleFactorParam() const;
    void setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle);

    // Getter and setter functions for interaction class FireMissile
    rti1516e::InteractionClassHandle getInteractionClassFireMissile() const;
    void setInteractionClassFireMissile(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamShooterID() const;
    void setParamShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamTargetID() const;
    void setParamTargetID(const rti1516e::ParameterHandle& handle);
    
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

    // Getter and setter functions for interaction class initiate handshake
    rti1516e::InteractionClassHandle getInteractionClassInitiateRedHandshake() const;
    void setInteractionClassInitiateRedHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::InteractionClassHandle getInteractionClassInitiateBlueHandshake() const;
    void setInteractionClassInitiateBlueHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamInitiateShooterID() const;
    void setParamInitiateShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateTargetID() const;
    void setParamInitiateTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateMissileAmountFired() const;
    void setParamInitiateMissileAmountFired(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateMaxMissilesRequired() const;
    void setParamInitiateMaxMissilesRequired(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateMissilesCurrentlyTargeting() const;
    void setParamInitiateMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateDistanceToTarget() const;
    void setParamInitiateDistanceToTarget(const rti1516e::ParameterHandle& handle);

    // Getter and setter functions for interaction class confirm handshake
    rti1516e::InteractionClassHandle getInteractionClassConfirmRedHandshake() const;
    void setInteractionClassConfirmRedHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::InteractionClassHandle getInteractionClassConfirmBlueHandshake() const;
    void setInteractionClassConfirmBlueHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamConfirmShooterID() const;
    void setParamConfirmShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmTargetID() const;
    void setParamConfirmTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmMissileAmountFired() const;
    void setParamConfirmMissileAmountFired(const rti1516e::ParameterHandle& handle);


    rti1516e::ParameterHandle getParamConfirmAllowFire() const;
    void setParamConfirmAllowFire(const rti1516e::ParameterHandle& handle);

    // Getters and setters for targetHit
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

    //Standard values get/set
    std::wstring getEnemyShipFederateName() const;
    void setEnemyShipFederateName(const std::wstring& name);

    ShipTeam getTeamStatus() const;
    void setTeamStatus(ShipTeam newStatus);

    std::pair<double, double> getEnemyShipPosition() const;
    void setEnemyShipPosition(const std::pair<double, double>& position);

    double getDistanceBetweenShips() const;
    void setDistanceBetweenShips(const double& distance);

    double getBearing() const;
    void setBearing(const double& bearing);

    bool getIsFiring() const;
    void setIsFiring(const bool& firing);

    // createShipsSyncPoint get/set
    bool getCreateShips() const;
    
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

    int getNumberOfMissiles() const;
    void setNumberOfMissiles(const int& numMissiles);

    //Sync label get
    std::wstring getSyncLabel() const;
    std::wstring getRedSyncLabel() const;
    std::wstring getBlueSyncLabel() const;

    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    //Enable time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    // Getter and setter for time management
    bool getIsRegulating() const;
    bool getIsConstrained() const;
    bool getIsAdvancing() const;
    void setIsAdvancing(bool advancing);

    // Getter and setter for shipMap
    std::unordered_map<rti1516e::ObjectInstanceHandle, Ship>& getShipMap();
    std::vector<Ship*>& getBlueShips();
    std::vector<Ship*>& getRedShips();
    std::vector<Ship*>& getOwnShips();

    const std::map<Ship*, Ship*>& getClosestEnemyShip();
    void setClosestEnemyShip(Ship* ship, Ship* target);
    void clearClosestEnemyShip();

    const std::vector<FireOrder>& getFireOrders() const;
    void clearFireOrders();

    // Map: distance to target -> (shipID, targetShipID)
    std::multimap<double, std::pair<Ship*, Ship*>> closestMissileRangeToTarget;

    void applyMissileLock(std::vector<Ship*>& shipVector, const std::wstring myTeam, const std::wstring& targetID, int numberOfMissilesFired);
    std::vector<Ship*>& getTargetShipVector(ShipTeam teamStatus, const std::wstring& shooterTeam);
};