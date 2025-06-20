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
#include "../include/jsonParse.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/MissileCalculator.h"
#include "../include/shipHelperFunctions.h"
#include "Ship.h"

class EnemyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiambassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";

    //Datavalues for setup
    int shipCounter = 0;
    int amountOfMissiles = 0;
    double timeScale = 0.0;

    //Standard values
    bool isFiring = false;
    double distanceBetweenShips = 0.0;
    double bearing = 0.0;
    std::wstring enemyShipFederateName = L"";
    std::pair<double, double> enemyShipPosition = std::make_pair(0.0, 0.0);

    //Handles for setup simulation interaction
    rti1516e::InteractionClassHandle setupSimulationHandle;
    rti1516e::ParameterHandle blueShips;
    rti1516e::ParameterHandle redShips;
    rti1516e::ParameterHandle timeScaleFactor;

    //Interaction send params and handle
    rti1516e::InteractionClassHandle fireRobotHandle;
    rti1516e::ParameterHandle fireParamHandle;
    rti1516e::ParameterHandle TargetParam;
    rti1516e::ParameterHandle startPosRobot;
    rti1516e::ParameterHandle targetPosition;

    //Handles for ship attributes
    rti1516e::ObjectClassHandle objectClassHandle;
    rti1516e::AttributeHandle attributeHandleMyShipPosition;
    rti1516e::AttributeHandle attributeHandleMyShipFederateName;
    rti1516e::AttributeHandle attributeHandleMyShipSpeed;
    rti1516e::AttributeHandle attributeHandleNumberOfMissiles;

    rti1516e::AttributeHandle attributeHandleEnemyShipFederateName;
    rti1516e::AttributeHandle attributeHandleEnemyShipPosition;

    void readJsonFile();

public: 
    EnemyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~EnemyShipFederateAmbassador();

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

    void receiveInteraction( // with time
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void receiveInteraction( // without time
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
    rti1516e::AttributeHandle getAttributeHandleMyShipPosition() const;
    void setAttributeHandleMyShipPosition(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMyShipFederateName() const;
    void setAttributeHandleMyShipFederateName(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleMyShipSpeed() const;
    void setAttributeHandleMyShipSpeed(const rti1516e::AttributeHandle& handle);

    rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles() const;
    void setAttributeHandleNumberOfMissiles(const rti1516e::AttributeHandle& handle);

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

    rti1516e::ParameterHandle getTargetPositionParam() const;
    void setTargetPositionParam(const rti1516e::ParameterHandle& handle);

    //Get and set for setup simulation interaction
    rti1516e::InteractionClassHandle getSetupSimulationHandle() const;
    void setSetupSimulationHandle(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getBlueShipsParam() const;
    void setBlueShipsParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getRedShipsParam() const;
    void setRedShipsParam(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getTimeScaleFactorParam() const;
    void setTimeScaleFactorParam(const rti1516e::ParameterHandle& handle);


    //Remove these when objectStructure is implemented
    //Getters and setters for ship attributes
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

    //Setup Values get/set  
    int getAmountOfShips() const;
    void setAmountOfShips(const int& amount);

    double getTimeScale() const;
    void setTimeScale(const double& scale);

    //Sync label get
    std::wstring getSyncLabel() const;
                            
    std::unordered_map<rti1516e::ObjectInstanceHandle, rti1516e::ObjectClassHandle> _shipInstances;
    //Enable time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    std::vector<Ship> ships;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> shipIndexMap;

    std::vector<EnemyShip> enemyShips;
    std::unordered_map<rti1516e::ObjectInstanceHandle, size_t> enemyShipIndexMap;



};