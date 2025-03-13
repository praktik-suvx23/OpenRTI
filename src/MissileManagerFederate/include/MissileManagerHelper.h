#ifndef MISSILEMANAGERHELPER_H
#define MISSILEMANAGERHELPER_H

#include "../MissileManagerFederateAmbassador.h"

/*
    IDEA:
    This is suppose to be a class that helps with the get/set methods for the MissileManagerFederateAmbassador.
    I belive the code got too bloated and hard to read with all the get/set methods in the MissileManagerFederateAmbassador.
    That is why I created this helper class to make it easier to read and understand the code.

    TODO:
    - Discuss if this is a good idea
    - One big class that does everything or split it up in smaller classes?
    - Placement of the helper class and name. New include folder in 'MissileManagerFederate'?
    - Move all used get/set methods from MissileManagerFederateAmbassador to this class
    - Clean up the get/set methods in MissileManagerFederateAmbassador
    - Make something similar for the Ships as well

    IMPORTANT:
    - Write a comment for each get/set group to explain where they are used to 
        make it easier to determine if they are still in use or not.
*/
class MissileManagerGetter {
    public:
    /* 1. Getter Methods for interaction class and parameters: HLAinteractionRoot.FireMissile */
    static rti1516e::InteractionClassHandle getFireMissileHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireMissileHandle;}
    static rti1516e::ParameterHandle getShooterIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterIDParamHandle;}
    static rti1516e::ParameterHandle getTargetIDParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetIDParamHandle;}
    static rti1516e::ParameterHandle getShooterPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shooterPositionParamHandle;}
    static rti1516e::ParameterHandle getTargetPositionParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.targetPositionParamHandle;}
    static rti1516e::ParameterHandle getMissileCountParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileCountParamHandle;}
    static rti1516e::ParameterHandle getMissileTypeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileTypeParamHandle;}
    static rti1516e::ParameterHandle getMaxDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.maxDistanceParamHandle;}
    static rti1516e::ParameterHandle getMissileSpeedParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.missileSpeedParamHandle;}
    static rti1516e::ParameterHandle getLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.lockOnDistanceParamHandle;}
    static rti1516e::ParameterHandle getFireTimeParamHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.fireTimeParamHandle;}

    /* 2. Getter Methods for object class and attributes: HLAobjectRoot.Ship 
    static rti1516e::ObjectClassHandle getShipClassHandle(MissileManagerAmbassador& ambassador) {
        return ambassador.shipClassHandle;}
    static rti1516e::AttributeHandle getAttributeHandleShipID(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipID;}
    static rti1516e::AttributeHandle getAttributeHandleShipTeam(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipTeam;}
    static rti1516e::AttributeHandle getAttributeHandleShipPosition(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipPosition;}
    static rti1516e::AttributeHandle getAttributeHandleShipSpeed(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipSpeed;}
    static rti1516e::AttributeHandle getAttributeHandleShipSize(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipSize;}
    static rti1516e::AttributeHandle getAttributeHandleShipAngle(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleShipAngle;}
    static rti1516e::AttributeHandle getAttributeHandleNumberOfMissiles(MissileManagerAmbassador& ambassador) {
        return ambassador.attributeHandleNumberOfMissiles;}*/
};

class MissileManagerSetter{
    public:
    /* 1.1 Setter Methods for interaction class and parameters: HLAinteractionRoot.FireMissile */
    static void setFireMissileHandle(MissileManagerAmbassador& ambassador, rti1516e::InteractionClassHandle handle) {
        ambassador.fireMissileHandle = handle;}
    static void setShooterIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterIDParamHandle = handle;}
    static void setTargetIDParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetIDParamHandle = handle;}
    static void setShooterPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.shooterPositionParamHandle = handle;}
    static void setTargetPositionParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.targetPositionParamHandle = handle;}
    static void setMissileCountParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileCountParamHandle = handle;}
    static void setMissileTypeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileTypeParamHandle = handle;}
    static void setMaxDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.maxDistanceParamHandle = handle;}
    static void setMissileSpeedParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.missileSpeedParamHandle = handle;}
    static void setLockOnDistanceParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.lockOnDistanceParamHandle = handle;}
    static void setFireTimeParamHandle(MissileManagerAmbassador& ambassador, rti1516e::ParameterHandle handle) {
        ambassador.fireTimeParamHandle = handle;}

    /* 2.1 Setter Methods for object class and attributes: HLAobjectRoot.Ship 
    static void setShipClassHandle(MissileManagerAmbassador& ambassador, rti1516e::ObjectClassHandle handle) {
        ambassador.shipClassHandle = handle;}
    static void setAttributeHandleShipID(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipID = handle;}
    static void setAttributeHandleShipTeam(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipTeam = handle;}
    static void setAttributeHandleShipPosition(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipPosition = handle;}
    static void setAttributeHandleShipSpeed(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipSpeed = handle;}
    static void setAttributeHandleShipSize(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipSize = handle;}
    static void setAttributeHandleShipAngle(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleShipAngle = handle;}
    static void setAttributeHandleNumberOfMissiles(MissileManagerAmbassador& ambassador, rti1516e::AttributeHandle handle) {
        ambassador.attributeHandleNumberOfMissiles = handle;}*/
};

#endif // MISSILEMANAGERHELPER_H