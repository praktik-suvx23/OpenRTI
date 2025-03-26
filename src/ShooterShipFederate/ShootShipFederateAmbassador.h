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

#include "../include/decodePosition.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/shipHelperFunctions.h"
#include "../include/MissileCalculator.h"
#include "Ship.h"

class MyShootShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiambassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";
    
    // Creating ship objects
    std::vector<Ship> friendlyShips;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> friendlyShipIndexMap;
    std::vector<EnemyShip> enemyShips;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> enemyShipIndexMap;

    // Variables related to time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    //Datavalues for setup
    int shipCounter = 1;
    int amountOfMissiles = 0;
    double timeScale = 0;

    //Json values
    std::wstring shipNumber;
    double shipheight;
    double shipwidth;
    double shiplength;
    double ShipSize;
    int numberOfMissiles;

    //Standard values
    bool isFiring = false;
    double distanceBetweenShips = 0.0;
    double bearing = 0.0;
    std::wstring _expectedShipName;

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

    rti1516e::AttributeHandle attributeHandleEnemyShipFederateName;
    rti1516e::AttributeHandle attributeHandleEnemyShipPosition;

    rti1516e::InteractionClassHandle interactionClassFireMissile;
    rti1516e::ParameterHandle parameterHandleShooterID;
    rti1516e::ParameterHandle parameterHandleMissileTeam;
    rti1516e::ParameterHandle parameterHandleMissileStartPosition;
    rti1516e::ParameterHandle parameterHandleMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleMissileSpeed;

    //Ship variables
    std::pair<double, double> myShipPosition = {0.0, 0.0};
    std::wstring myShipFederateName = L"";
    double myShipSpeed = 0.0;

    std::pair<double, double> enemyShipPosition = {0.0, 0.0};
    std::wstring enemyShipFederateName = L"";
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

    // Getters and setters for time management
    std::chrono::time_point<std::chrono::high_resolution_clock> getStartTime() const;
    void setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time);
    bool getIsRegulating() const;
    bool getIsConstrained() const;
    bool getIsAdvancing() const;
    void setIsAdvancing(bool advancing);

    // Getter and setter for ship objects
    std::vector<Ship>& getFriendlyShips();
    void setFriendlyShips(const std::vector<Ship>& ships);
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> getFriendlyShipIndexMap();
    void setFriendlyShipIndexMap(const std::unordered_map<rti1516e::ObjectInstanceHandle, size_t>& indexMap);

    std::vector<EnemyShip>& getEnemyShips();
    void setEnemyShips(const std::vector<EnemyShip>& ships);
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> getEnemyShipIndexMap();
    void setEnemyShipIndexMap(const std::unordered_map<rti1516e::ObjectInstanceHandle, size_t>& indexMap);

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

    // Getters and setters for Enemy ship attributeshandles
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

    //Getters and setters for ship attributes
    std::pair<double, double> getMyShipPosition() const;
    void setMyShipPosition(const std::pair<double, double>& position);

    std::wstring getMyShipFederateName() const;
    void setMyShipFederateName(const std::wstring& name);

    double getMyShipSpeed() const;
    void setMyShipSpeed(const double& speed);

    std::wstring getEnemyShipFederateName() const;
    void setEnemyShipFederateName(const std::wstring& name);

    std::pair<double, double> getEnemyShipPosition() const;
    void setEnemyShipPosition(const std::pair<double, double>& position);

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

    int getNumberOfMissiles() const;
    void setNumberOfMissiles(const int& numMissiles);

    //Sync label get
    std::wstring getSyncLabel() const;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    
    //std::vector<rti1516e::ObjectInstanceHandle> objectInstanceHandles; // ??
};