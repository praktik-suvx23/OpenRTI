#ifndef PYLINKAMBASSADOR_H
#define PYLINKAMBASSADOR_H

#include <RTI/NullFederateAmbassador.h>

#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>
#include <chrono>

#include "../../include/decodePosition.h"
#include "../../include/ObjectInstanceHandleHash.h"
#include "../../CombinedShip/Ship.h"
#include "../../MissileFederate/structMissile.h"

class PyLinkAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiAmbassador;
    // Variables related to time management
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    // For synchronization
    std::wstring syncLabel = L"";

    // discoverObjectInstance counter: keep track of the number of discovered objects
    uint16_t numberOfDiscoveredObjects = 0;

    // Variables related to ship objects
    std::vector<Ship> redShips;
    std::vector<Ship> blueShips;
    std::vector<Missile> missiles;
    std::unordered_map<rti1516e::ObjectInstanceHandle, uint16_t> discoveredObjects;

    // Handles for object class and attributes
    rti1516e::ObjectClassHandle objectClassHandleShip;
    rti1516e::AttributeHandle attributeHandleShipID;
    rti1516e::AttributeHandle attributeHandleShipTeam;
    rti1516e::AttributeHandle attributeHandleShipPosition;
    rti1516e::AttributeHandle attributeHandleShipSpeed;
    rti1516e::AttributeHandle attributeHandleShipSize;
    rti1516e::AttributeHandle attributeHandleShipHP;

    rti1516e::ObjectClassHandle objectClassHandleMissile;
    rti1516e::AttributeHandle attributeHandleMissileID;
    rti1516e::AttributeHandle attributeHandleMissileTeam;
    rti1516e::AttributeHandle attributeHandleMissilePosition;
    rti1516e::AttributeHandle attributeHandleMissileAltitude;
    rti1516e::AttributeHandle attributeHandleMissileSpeed;

public:
    PyLinkAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~PyLinkAmbassador();

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

    void updateOrInsertShip(std::vector<Ship>& shipVec, Ship& ship);
    void updateOrInsertMissile(std::vector<Missile>& missileVec, Missile& missile);

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

    //Synchronization point
    std::wstring getSyncLabel() const;

    // Ship object related functions
    std::vector<Ship>& getRedShips();
    std::vector<Ship>& getBlueShips();
    std::vector<Missile>& getMissiles();

    // Getters and setters for object class Ship and its attributes
    rti1516e::ObjectClassHandle getObjectClassHandleShip() const;
    void setObjectClassHandleShip(const rti1516e::ObjectClassHandle& handle);
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
    rti1516e::AttributeHandle getAttributeHandleShipHP() const;
    void setAttributeHandleShipHP(const rti1516e::AttributeHandle& handle);

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
};

#endif // PYLINKAMBASSADOR_H