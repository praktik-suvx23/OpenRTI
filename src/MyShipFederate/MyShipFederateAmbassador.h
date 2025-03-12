#ifndef MYSHIPFEDERATEAMBASSADOR_H
#define MYSHIPFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>

#include <iostream>

class MyShipFederateAmbassador : public rti1516e::NullFederateAmbassador {
    rti1516e::RTIambassador* _rtiAmbassador;
    std::wstring federateName = L"";
    std::wstring syncLabel = L"";

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
    MyShipFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MyShipFederateAmbassador();

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

    //Sync label get
    std::wstring getSyncLabel() const;

    // Setters and getters for ship attributes
    void setFederateName(std::wstring name);
    std::wstring getFederateName() const;

    void setshipNumber(std::wstring name);
    std::wstring getshipNumber() const;

    void setshipheight(double height);
    double getshipheight() const;

    void setshipwidth(double width);
    double getshipwidth() const ;

    void setshiplength(double length);
    double getshiplength() const;

    void setNumberOfRobots(int numRobots);
    int getNumberOfRobots() const;

    void setShipSize(double size);
    double getShipSize();

    
    bool isRegulating = false;  
    bool isConstrained = false;
    bool isAdvancing = false;

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    rti1516e::ObjectInstanceHandle objectInstanceHandle; // Give set/get methods for this

    // ObjectClassHandle set and get. Used in registerShipObject
    rti1516e::ObjectClassHandle getMyObjectClassHandle() const;
    void setMyObjectClassHandle(rti1516e::ObjectClassHandle handle);

    private:
    rti1516e::ObjectClassHandle objectClassHandle;
    std::wstring shipNumber;
    double shipheight;
    double shipwidth;
    double shiplength;
    double ShipSize;
    int numberOfRobots;


   

};

#endif